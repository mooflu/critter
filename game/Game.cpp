// Description:
//   High level infrastructure for game.
//
// Copyright (C) 2001 Frank Becker
//
// This program is free software; you can redistribute it and/or modify it under
// the terms of the GNU General Public License as published by the Free Software
// Foundation;  either version 2 of the License,  or (at your option) any  later
// version.
//
// This program is distributed in the hope that it will be useful,  but  WITHOUT
// ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
// FOR A PARTICULAR PURPOSE. See the GNU General Public License for more details
//
#include <Trace.hpp>

#include <Game.hpp>
#include <GameState.hpp>
#include <Constants.hpp>
#include <Config.hpp>
#include <PausableTimer.hpp>
#include <ScoreKeeper.hpp>

#include <Audio.hpp>
#include <Video.hpp>
#include <Input.hpp>

#include <Hero.hpp>
#include <WeaponDepot.hpp>
#include <ParticleGroup.hpp>
#include <ParticleGroupManager.hpp>
#include <Starfield.hpp>

#include <LPathManager.hpp>
#include <ModelManager.hpp>
#include <StageManager.hpp>
#include <MenuManager.hpp>
#include <ResourceManager.hpp>

#if defined(EMSCRIPTEN)
#include <emscripten.h>
#include <emscripten/html5.h>
#endif

Game::Game(void) {
    XTRACE();
}

Game::~Game() {
    XTRACE();

    LOG_INFO << "Shutting down..." << endl;

    // save config stuff
    ConfigS::instance()->saveToFile();

    // save leaderboard
    ScoreKeeperS::instance()->save();

    StarfieldS::cleanup();

    // misc cleanup
    MenuManagerS::cleanup();
    StageManagerS::cleanup();
    InputS::cleanup();
    AudioS::cleanup();
    VideoS::cleanup();  //Video calls SDL_Quit
    LPathManagerS::cleanup();
    ParticleGroupManagerS::cleanup();
    WeaponDepotS::cleanup();
    ScoreKeeperS::cleanup();

    HeroS::cleanup();  //has to be after ParticleGroupManager

    // Note: this shuts down PHYSFS
    LOG_INFO << "ResourceManager cleanup..." << endl;
    ResourceManagerS::cleanup();
}

bool Game::init(void) {
    XTRACE();
    bool result = true;

    ScoreKeeperS::instance()->load();

    // init subsystems et al
    if (!ParticleGroupManagerS::instance()->init()) {
        return false;
    }

    AudioS::instance()->setDefaultSoundtrack("music/lg-criti.ogg");
    if (!AudioS::instance()->init()) {
        return false;
    }

    if (!VideoS::instance()->init()) {
        return false;
    }
    if (!InputS::instance()->init()) {
        return false;
    }
    if (!MenuManagerS::instance()->init()) {
        return false;
    }
    if (!HeroS::instance()->init()) {
        return false;
    }

    StarfieldS::instance()->init(-150.0);

    ParticleGroupManager* pgm = ParticleGroupManagerS::instance();
    if (!pgm->init()) {
        return false;
    }

    //init all the paricle groups and links between them
    pgm->addGroup(HERO_GROUP, 1);
    pgm->addGroup(ENEMIES_GROUP, 300);
    pgm->addGroup(ENEMIES_BULLETS_GROUP, 300);

    //there are 3 effect groups to give very simple control over the order
    //of drawing which is important for alpha blending.
    pgm->addGroup(EFFECTS_GROUP1, 1000);
    pgm->addGroup(EFFECTS_GROUP2, 1000);
    pgm->addGroup(EFFECTS_GROUP3, 1000);

    pgm->addGroup(HERO_BULLETS_GROUP, 300);
    pgm->addGroup(BONUS_GROUP, 50);
    pgm->addGroup(SHOOTABLE_ENEMY_BULLETS_GROUP, 100);
    pgm->addGroup(SHOOTABLE_BONUS_GROUP, 50);

    //collision detect between the following groups
    pgm->addLink(HERO_BULLETS_GROUP, SHOOTABLE_ENEMY_BULLETS_GROUP);
    pgm->addLink(HERO_BULLETS_GROUP, ENEMIES_GROUP);
    pgm->addLink(HERO_BULLETS_GROUP, SHOOTABLE_BONUS_GROUP);
    pgm->addLink(HERO_GROUP, ENEMIES_GROUP);
    pgm->addLink(HERO_GROUP, ENEMIES_BULLETS_GROUP);
    pgm->addLink(HERO_GROUP, BONUS_GROUP);
    pgm->addLink(HERO_GROUP, SHOOTABLE_ENEMY_BULLETS_GROUP);
    pgm->addLink(HERO_GROUP, SHOOTABLE_BONUS_GROUP);

    //reset our stopwatch
    GameState::stopwatch.reset();
    GameState::stopwatch.pause();

    if (!StageManagerS::instance()->init()) {
        return false;
    }

    ConfigS::instance()->getFloat("horsePower", GameState::horsePower);

    //add our hero...
    ParticleGroupManagerS::instance()->getParticleGroup(HERO_GROUP)->newParticle(string("Hero"), 0, 0, -100);

    //make sure we start of in menu mode
    MenuManagerS::instance()->turnMenuOn();

    GameState::startOfStep = Timer::getTime();
    GameState::startOfGameStep = GameState::stopwatch.getTime();

    LOG_INFO << "Initialization complete OK." << endl;

    return result;
}

void Game::reset(void) {
    //reset in order to start new game
    ParticleGroupManagerS::instance()->reset();
    HeroS::instance()->reset();
    ParticleGroupManagerS::instance()->getParticleGroup(HERO_GROUP)->newParticle(string("Hero"), 0, 0, -100);

    ScoreKeeperS::instance()->resetCurrentScore();
    GameState::startOfGameStep = GameState::stopwatch.getTime();
    StageManagerS::instance()->reset();
}

void Game::startNewGame(void) {
    GameS::instance()->reset();
    GameState::context = Context::eInGame;
    SDL_SetRelativeMouseMode(SDL_TRUE);
    InputS::instance()->disableInterceptor();
    AudioS::instance()->playSample("sounds/voiceGo.wav");

    bool allowVerticalMovement = false;
    ConfigS::instance()->getBoolean("allowVerticalMovement", allowVerticalMovement);
    HeroS::instance()->allowVerticalMovement(allowVerticalMovement);
}

void Game::updateOtherLogic(void) {
    int stepCount = 0;
    float currentTime = Timer::getTime();
    while ((currentTime - GameState::startOfStep) > GAME_STEP_SIZE) {
        StarfieldS::instance()->update();

        //FIXME: shouldn't run all the time...
        MenuManagerS::instance()->update();

        //advance to next start-of-game-step point in time
        GameState::startOfStep += GAME_STEP_SIZE;
        currentTime = Timer::getTime();

        stepCount++;
        if (stepCount > MAX_GAME_STEPS) {
            break;
        }
    }

    GameState::frameFractionOther = (currentTime - GameState::startOfStep) / GAME_STEP_SIZE;

    if (stepCount > 1) {
        //LOG_WARNING << "Skipped " << stepCount << " frames." << endl;

        if (GameState::frameFractionOther > 1.0) {
            //Our logic is still way behind where it should be at this
            //point in time. If we get here we already ran through
            //MAX_GAME_STEPS logic runs trying to catch up.

            //We clamp the value to 1.0, higher values would try
            //to predict were we are visually. Maybe not a bad idead
            //to allow values up to let's say 1.3...
            GameState::frameFractionOther = 1.0;
        }
    }
}

void Game::updateInGameLogic(void) {
    int stepCount = 0;
    float currentGameTime = GameState::stopwatch.getTime();
    while ((currentGameTime - GameState::startOfGameStep) > GAME_STEP_SIZE) {
        // adding new enemies, moving on to the next level, etc.
        StageManagerS::instance()->update();
        // update all objects, particles, etc.
        ParticleGroupManagerS::instance()->update();

        //FIXME: Currently the Critterboard is updated in the video system. Should be on its own.
        VideoS::instance()->updateLogic();

        //advance to next start-of-game-step point in time
        GameState::startOfGameStep += GAME_STEP_SIZE;
        currentGameTime = GameState::stopwatch.getTime();

        stepCount++;
        if (stepCount > MAX_GAME_STEPS) {
            break;
        }
    }

    GameState::frameFraction = (currentGameTime - GameState::startOfGameStep) / GAME_STEP_SIZE;

    if (stepCount > 1) {
        if (GameState::frameFraction > 1.0) {
            //Our logic is still way behind where it should be at this
            //point in time. If we get here we already ran through
            //MAX_GAME_STEPS logic runs trying to catch up.

            //We clamp the value to 1.0, higher values would try
            //to predict were we are visually. Maybe not a bad idead
            //to allow values up to let's say 1.3...
            GameState::frameFraction = 1.0;
        }
    }
}

void Game::gameLoop(void) {
    XTRACE();
    Game& game = *GameS::instance();

    Audio& audio = *AudioS::instance();
    Video& video = *VideoS::instance();
    Input& input = *InputS::instance();

    switch (GameState::context) {
        case Context::eInGame:
            //stuff that only needs updating when game is actually running
            game.updateInGameLogic();
            break;

        default:
            break;
    }

    //stuff that should run all the time
    game.updateOtherLogic();

    input.update();
    audio.update();
    video.update();

    GLenum err;
    while ((err = glGetError()) != GL_NO_ERROR) {
        LOG_ERROR << "GL ERROR: " << std::hex << err << "\n";
    }

#if defined(EMSCRIPTEN)
    if (!GameState::isAlive) {
        GameS::cleanup();
        ConfigS::cleanup();

        EM_ASM(console.log('FS.syncfs...'); FS.syncfs(function(err) {
            if (err) {
                console.log('FS.syncfs error: ' + err)
            }
            console.log('syncfs done');
            Module.gamePostRun();
        }););

        emscripten_exit_pointerlock();
        emscripten_cancel_main_loop();
        //emscripten_exit_with_live_runtime();
    }
#endif
}

void Game::run(void) {
    XTRACE();

    // Here it is: the main loop.
    LOG_INFO << "Entering Main loop." << endl;
#if defined(IPHONE)
    Game::gameLoop();
#else
    while (GameState::isAlive) {
        Game::gameLoop();
    }
#endif
}
