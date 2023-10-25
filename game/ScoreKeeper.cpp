// Description:
//   Score keeper.
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
#include <iostream>
#include <string>
#include <algorithm>

#include <Trace.hpp>
#include <Config.hpp>
#include <Tokenizer.hpp>
#include <zStream.hpp>
#include <ScoreKeeper.hpp>
#include <Constants.hpp>
#include <GameState.hpp>
#include <GLBitmapFont.hpp>
#include <FontManager.hpp>

#include <ParticleGroup.hpp>
#include <ParticleGroupManager.hpp>

//using namespace std;

const int LEADERBOARD_SIZE = 11;  //top-10 plus current score

ScoreKeeper::ScoreKeeper(void) :
    _currentIndex(LEADERBOARD_SIZE - 1),
    _leaderBoard(LEADERBOARD_SIZE) {
    XTRACE();
    for (int i = 0; i < LEADERBOARD_SIZE; i++) {
        _leaderBoard[i].score = 1000 * (10 - i);
        switch (i % 4) {
            case 0:
                _leaderBoard[i].name = "Critical-Mass";
                break;
            case 1:
                _leaderBoard[i].name = "mooflu.com";
                break;
            case 2:
                _leaderBoard[i].name = "FB";
                break;
            default:
                _leaderBoard[i].name = "SliQ";
                break;
        }
        time(&_leaderBoard[i].time);
        _leaderBoard[i].goodiesMissed = 0;
        _leaderBoard[i].goodiesCaught = 0;
        _leaderBoard[i].skill = (int)Skill::eRookie;
    }
}

ScoreKeeper::~ScoreKeeper(void) {
    XTRACE();
}

void ScoreKeeper::resetCurrentScore(void) {
    _currentIndex = LEADERBOARD_SIZE - 1;
    _leaderBoard[_currentIndex].score = 0;
    _leaderBoard[_currentIndex].name = "???";
    _leaderBoard[_currentIndex].time = 0;
}

int ScoreKeeper::addToCurrentScore(int value) {
    XTRACE();
    float multiplier = 1.0f;

    switch (GameState::skill) {
        case Skill::eRookie:
            multiplier = 0.5f;
            break;

        case Skill::eNormal:
            multiplier = 1.0f;
            break;

        case Skill::eExpert:
            multiplier = 1.5f;
            break;

        case Skill::eInsane:
            multiplier = 3.0f;
            break;

        case Skill::eUnknown:
        case Skill::eLAST:
        default:
            break;
    }

    int newValue = (int)((float)value * multiplier);

    _leaderBoard[_currentIndex].score += newValue;
    time(&_leaderBoard[_currentIndex].time);
    _leaderBoard[_currentIndex].skill = (int)GameState::skill;

    updateLeaderBoard();

    //return the real value;
    return newValue;
}

void ScoreKeeper::incGoodiesCaught(void) {
    _leaderBoard[_currentIndex].goodiesCaught++;
}

int ScoreKeeper::goodiesCaught(void) {
    return _leaderBoard[_currentIndex].goodiesCaught;
}

void ScoreKeeper::incGoodiesMissed(void) {
    _leaderBoard[_currentIndex].goodiesMissed++;
}

void ScoreKeeper::updateLeaderBoard(void) {
    XTRACE();

    if (_currentIndex == 0) {
        return;  //we are leader, no update required
    }

    bool newPos = false;
    ScoreData tmpData = _leaderBoard[_currentIndex];
    while ((_currentIndex > 0) && (tmpData.score > _leaderBoard[_currentIndex - 1].score)) {
        _leaderBoard[_currentIndex] = _leaderBoard[_currentIndex - 1];
        _currentIndex--;
        newPos = true;
    }
    _leaderBoard[_currentIndex] = tmpData;

    if (newPos) {
        static ParticleGroup* effects = ParticleGroupManagerS::instance()->getParticleGroup(EFFECTS_GROUP2);
        ParticleInfo pi;
        pi.position.x = 0.0;
        pi.position.y = -20 + 3.5f * (_currentIndex + 1);
        pi.position.z = -50.0f;
        char buf[128];
        sprintf(buf, "You've climbed to #%d on the leader board!", _currentIndex + 1);
        pi.text = buf;

        pi.color.x = 0.2f;
        pi.color.y = 1.0f;
        pi.color.z = 0.2f;

        pi.extra.y = 0.05f;
        pi.extra.z = 0.05f;

        effects->newParticle("StatusMessage", pi);
    }

    //    dump();
}

void ScoreKeeper::dump(void) {
    LOG_INFO << "------LeaderBoard-----" << endl;
    for (unsigned int i = 0; i < _leaderBoard.size(); i++) {
        LOG_INFO.width(30);
        LOG_VOID.fill('.');
        LOG_VOID.unsetf(ios::right);
        LOG_VOID.setf(ios::left);
        LOG_VOID << _leaderBoard[i].name.c_str();

        LOG_VOID.width(10);
        LOG_VOID.unsetf(ios::left);
        LOG_VOID.setf(ios::right);
        LOG_VOID << _leaderBoard[i].score;

        char buf[128];
        strftime(buf, 127, "%a %d-%b-%Y %H:%M", localtime(&_leaderBoard[i].time));
        LOG_VOID << " : " << buf;

        int totalGoodies = _leaderBoard[i].goodiesCaught + _leaderBoard[i].goodiesMissed;
        if (totalGoodies > 0) {
            sprintf(buf, " (%d%%)", (_leaderBoard[i].goodiesCaught * 100) / totalGoodies);
            LOG_VOID << buf;
        }

        LOG_VOID << endl;
    }
}

const string ScoreKeeper::getInfoText(unsigned int index) {
    string info = "";

    if (index < _leaderBoard.size()) {
        info = _leaderBoard[index].name;
        info += " made this run on ";

        char buf[128];
        strftime(buf, 127, "%a %d-%b-%Y %H:%M", localtime(&_leaderBoard[index].time));
        info += buf;

        int totalGoodies = _leaderBoard[index].goodiesCaught + _leaderBoard[index].goodiesMissed;
        if (totalGoodies > 0.0f) {
            sprintf(buf, " (%d%%)", (_leaderBoard[index].goodiesCaught * 100) / totalGoodies);
            info += buf;
        }
        info += " on " + Skill::getString((Skill::SkillEnum)_leaderBoard[index].skill);
    }

    return info;
}

void ScoreKeeper::load(void) {
    XTRACE();
    string scoreFilename = ConfigS::instance()->getConfigDirectory();
    scoreFilename += "leaderboard";
    LOG_INFO << "Loading hi-scores from " << scoreFilename << endl;

    int version = 1;
    ziStream infile(scoreFilename.c_str());
    if (infile.isOK()) {
        _leaderBoard.clear();

        string line;
        while (!getline(infile, line).eof()) {
            //	    LOG_INFO << "[" << line << "]" << endl;
            //explicitly skip comments
            if (line[0] == '#') {
                continue;
            }

            ScoreData sData;
            Tokenizer t(line, false, " \t\n\r");
            string token = t.next();
            if (token == "Version") {
                string versionString = t.next();
                version = atoi(versionString.c_str());
            } else {
                switch (version) {
                    case 1:
                        sData.name = token;
                        sData.score = atoi(t.next().c_str());
                        sData.time = (time_t)atoi(t.next().c_str());
                        sData.goodiesCaught = 0;
                        sData.goodiesMissed = 0;
                        sData.skill = (int)Skill::eRookie;
                        _leaderBoard.push_back(sData);
                        break;

                    case 2:
                        sData.name = token;
                        sData.score = atoi(t.next().c_str());
                        sData.time = (time_t)atoi(t.next().c_str());
                        sData.goodiesCaught = atoi(t.next().c_str());
                        sData.goodiesMissed = atoi(t.next().c_str());
                        sData.skill = (Skill::SkillEnum)atoi(t.next().c_str());
                        _leaderBoard.push_back(sData);
                        break;

                    default:
                        break;
                }
            }
        }
    }

    //should already be sorted, but better be safe...
    sort(_leaderBoard.begin(), _leaderBoard.end());

    dump();
}

void ScoreKeeper::save(void) {
    XTRACE();

    string scoreFilename = ConfigS::instance()->getConfigDirectory();
    scoreFilename += "leaderboard";
    LOG_INFO << "Saving hi-scores to " << scoreFilename << endl;

    //Save scores in a compressed file to make it a bit tougher to cheat...
    zoStream zout(scoreFilename.c_str());
    if (zout.good()) {
        zout << "#------LeaderBoard-----#\n";
        zout << "Version 2\n";

        for (unsigned int i = 0; i < _leaderBoard.size(); i++) {
            if (_leaderBoard[i].name == "") {
                _leaderBoard[i].name = "Anonymous";
            }
            zout << _leaderBoard[i].name << " " << _leaderBoard[i].score << " " << (long)_leaderBoard[i].time << " "
                 << _leaderBoard[i].goodiesCaught << " " << _leaderBoard[i].goodiesMissed << " "
                 << _leaderBoard[i].skill << " "
                 << "\n";
        }
    }
}

void ScoreKeeper::draw(const Point2Di& /*offset*/) {
    static GLBitmapFont* fontWhite = FontManagerS::instance()->getFont("bitmaps/menuWhite");
    static GLBitmapFont* fontShadow = FontManagerS::instance()->getFont("bitmaps/menuShadow");

    for (unsigned int i = 0; i < (_leaderBoard.size() - 1); i++) {
        char buf[128];
        sprintf(buf, "%d", _leaderBoard[i].score);

        char buf2[10];
        sprintf(buf2, "%d.", i + 1);

        float width = fontWhite->GetWidth(buf, 1.0);

        fontShadow->setColor(1.0, 1.0, 1.0, 1.0);
        fontShadow->DrawString(buf2, 250 + 7, 420 - (float)i * 36 - 7, 1.0f, 1.0f);
        fontShadow->DrawString(_leaderBoard[i].name.c_str(), 300 + 7, 420 - (float)i * 36 - 7, 1.0f, 1.0f);
        fontShadow->DrawString(buf, 750 - width + 9, 420 - (float)i * 36 - 9, 1.0f, 1.0f);

        if (i == _currentIndex) {
            fontWhite->setColor(1.0, 0.852f, 0.0, 1.0);
        } else {
            fontWhite->setColor(1.0, 1.0, 1.0, 1.0);
        }

        fontWhite->DrawString(buf2, 250, 420 - (float)i * 36, 1.0f, 1.0f);
        fontWhite->DrawString(_leaderBoard[i].name.c_str(), 300, 420 - (float)i * 36, 1.0f, 1.0f);
        fontWhite->DrawString(buf, 750 - width, 420 - (float)i * 36, 1.0f, 1.0f);
    }
}
