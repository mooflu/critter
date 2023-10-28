// Description:
//   Video subsystem.
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
#include "Video.hpp"

#include "SDL.h"
#include <math.h>

#include <FPS.hpp>

#include <PNG.hpp>
#include <Trace.hpp>
#include <Config.hpp>
#include <Value.hpp>
#include <GameState.hpp>
#include <Constants.hpp>

#include <Hero.hpp>
#include <ParticleGroupManager.hpp>
#include <Timer.hpp>
#include <Camera.hpp>
#include <Starfield.hpp>
#include <MenuManager.hpp>
#include <StageManager.hpp>
#include <FontManager.hpp>
#include <BitmapManager.hpp>
#include <ModelManager.hpp>
#include <ScoreKeeper.hpp>
#include <TextureManager.hpp>
#include <GLExtension.hpp>
#include "Input.hpp"

#include <ResourceManager.hpp>
#include <zrwops.hpp>

#include <GL/glew.h>
#include "gl3/Program.hpp"
#include "gl3/Shader.hpp"
#include "gl3/Buffer.hpp"
#include "gl3/VertexArray.hpp"
#include "gl3/ProgramManager.hpp"
#include "gl3/MatrixStack.hpp"

#include "glm/glm.hpp"
#include "glm/ext.hpp"
#include "GLVertexBufferObject.hpp"

#include <algorithm>
#include <string>
#include <locale>

using namespace std;

//Earlier versions of SDL don't have DISABLE defined
#ifndef SDL_DISABLE
#define SDL_DISABLE 0
#endif

Video::Video() :
    _isFullscreen(false),
    _showStarfield(true),
    _showNebulas(true),
    _maxFPS(0),
    _fpsStepSize(0.0),
    _bpp(0),
    _width(VIDEO_DEFAULT_WIDTH),
    _height(VIDEO_DEFAULT_HEIGHT),
    _boardVisible(true),
    _boardPosX(0),
    _titleA(0),
    _titleB(0),
    _angle(0.0),
    _prevAngle(0.0),
    _windowHandle(0),
    _glContext(0) {
    XTRACE();
}

Video::~Video() {
    XTRACE();
    LOG_INFO << "Video shutdown..." << endl;

    BitmapManagerS::cleanup();
    FontManagerS::cleanup();
    ModelManagerS::cleanup();

    TextureManagerS::cleanup();
    GLExtension::close();

    delete _titleA;
    delete _titleB;

    SkillS::cleanup();
    CameraS::cleanup();

    SDL_QuitSubSystem(SDL_INIT_VIDEO);
    SDL_Quit();
}

void Video::reset(void) {
    BitmapManagerS::instance()->reset();
    FontManagerS::instance()->reset();
    ModelManagerS::instance()->reset();
    MenuManagerS::instance()->reset();

    _titleA->reset();
    _titleB->reset();

    ProgramManagerS::instance()->reset();
}

void Video::reload(void) {
    initGL3Test(); // re-creates shaders

    BitmapManagerS::instance()->reload();
    FontManagerS::instance()->reload();
    ModelManagerS::instance()->reload();
    MenuManagerS::instance()->reload();

    _titleA->reload();
    _titleB->reload();
}

void Video::initGL3Test() {
    while (!MatrixStack::model.empty()) {
        MatrixStack::model.pop();
    }
    while (!MatrixStack::projection.empty()) {
        MatrixStack::projection.pop();
    }

    MatrixStack::model.push(glm::mat4(1.0f));
    MatrixStack::projection.push(glm::mat4(1.0f));

    Program* progLight = ProgramManagerS::instance()->createProgram("lighting");
    progLight->use();
    progLight->release();

    Program* progTexture = ProgramManagerS::instance()->createProgram("texture");
    progTexture->use();
    progTexture->release();

    LOG_INFO << "initGL3Test DONE\n";
}

bool Video::init(void) {
    XTRACE();
    LOG_INFO << "Initializing Video..." << endl;

    if (SDL_InitSubSystem(SDL_INIT_VIDEO) < 0) {
        LOG_ERROR << "Init Video: failed # " << SDL_GetError() << endl;
        return false;
    }
    LOG_INFO << "Video: OK" << endl;

    ConfigS::instance()->getInteger("maxFPS", _maxFPS);
    if (_maxFPS) {
        LOG_INFO << "Video: Restricting FPS to " << _maxFPS << endl;
        _fpsStepSize = 1.0f / (float)_maxFPS;
    }

    ConfigS::instance()->getBoolean("fullscreen", _isFullscreen);

    if (!setVideoMode()) {
        SDL_QuitSubSystem(SDL_INIT_VIDEO);
        return false;
    }

    initGL3Test();

    _smallFont = FontManagerS::instance()->getFont("bitmaps/arial-small");
    if (!_smallFont) {
        LOG_ERROR << "Unable to get font... (arial-small)" << endl;
        SDL_QuitSubSystem(SDL_INIT_VIDEO);
        return false;
    }

    _scoreFont = FontManagerS::instance()->getFont("bitmaps/vipnaUpper");
    if (!_scoreFont) {
        LOG_ERROR << "Unable to get font... (vipnaUpper)" << endl;
        SDL_QuitSubSystem(SDL_INIT_VIDEO);
        return false;
    }

    _gameOFont = FontManagerS::instance()->getFont("bitmaps/gameover");
    if (!_gameOFont) {
        LOG_ERROR << "Unable to get font... (gameover)" << endl;
        SDL_QuitSubSystem(SDL_INIT_VIDEO);
        return false;
    }

    _board = BitmapManagerS::instance()->getBitmap("bitmaps/board");
    if (!_board) {
        LOG_ERROR << "Unable to load CritterBoard" << endl;
        SDL_QuitSubSystem(SDL_INIT_VIDEO);
        return false;
    }
    _boardIndex = _board->getIndex("CritterBoard");

    std::unique_ptr<ziStream> titleAP(ResourceManagerS::instance()->getInputStream("bitmaps/titleA.png"));
    if (!titleAP) {
        LOG_WARNING << "titleA.png not found." << endl;
        return false;
    }
    ziStream& bminfile1 = *titleAP;
    SDL_RWops* src = RWops_from_ziStream(bminfile1);
    SDL_Surface* img1 = IMG_LoadPNG_RW(src);
    SDL_RWclose(src);
    _titleA = new GLTexture(GL_TEXTURE_2D, img1, false);

    std::unique_ptr<ziStream> titleBP(ResourceManagerS::instance()->getInputStream("bitmaps/titleB.png"));
    if (!titleBP) {
        LOG_WARNING << "titleB.png not found." << endl;
        return false;
    }
    ziStream& bminfile2 = *titleBP;
    src = RWops_from_ziStream(bminfile2);
    SDL_Surface* img2 = IMG_LoadPNG_RW(src);
    SDL_RWclose(src);
    _titleB = new GLTexture(GL_TEXTURE_2D, img2, false);

#if 0
    glViewport(0, 0, _width, _height);

    GLfloat mat_ambient[] = {0.5f, 0.5f, 0.5f, 0.5f};
    GLfloat mat_diffuse[] = {0.4f, 0.4f, 0.4f, 0.5f};
    GLfloat mat_specular[] = {0.6f, 0.6f, 0.6f, 0.5f};
    GLfloat mat_shininess[] = {60.0f};
    glMaterialfv(GL_FRONT, GL_SPECULAR, mat_specular);
    glMaterialfv(GL_FRONT, GL_SHININESS, mat_shininess);
    glMaterialfv(GL_FRONT, GL_AMBIENT, mat_ambient);
    glMaterialfv(GL_FRONT, GL_DIFFUSE, mat_diffuse);

    GLfloat mat_diffuse_b[] = {0.5f, 0.5f, 0.5f, 0.5f};
    GLfloat mat_ambient_b[] = {0.3f, 0.3f, 0.3f, 0.5f};
    GLfloat mat_specular_b[] = {0.2f, 0.2f, 0.2f, 0.5f};
    GLfloat mat_shininess_b[] = {10.0f};
    glMaterialfv(GL_BACK, GL_SPECULAR, mat_specular_b);
    glMaterialfv(GL_BACK, GL_SHININESS, mat_shininess_b);
    glMaterialfv(GL_BACK, GL_AMBIENT, mat_ambient_b);
    glMaterialfv(GL_BACK, GL_DIFFUSE, mat_diffuse_b);

    glClearColor(0.0, 0.0, 0.0, 0.0);
#endif

    return true;
}

#define DEBUG_OPENGL 1
#if defined(DEBUG_OPENGL) && !defined(EMSCRIPTEN)
void GLAPIENTRY
MessageCallback(GLenum source,
    GLenum type,
    GLuint id,
    GLenum severity,
    GLsizei length,
    const GLchar* message,
    const void* userParam)
{
    if (severity == GL_DEBUG_SEVERITY_LOW || severity == GL_DEBUG_SEVERITY_NOTIFICATION) {
        return;
    }
    LOG_INFO << "GL Error: " << message << " - type: " << hex << type << " - severity: " << severity << "\n";
}
#endif

bool Video::setVideoMode(void) {
    ConfigS::instance()->getBoolean("fullscreen", _isFullscreen);
    ConfigS::instance()->getInteger("width", _width);
    ConfigS::instance()->getInteger("height", _height);
    _prevWidth = _width;
    _prevHeight = _height;

    if ((_width == 0) && (_height == 0)) {
        SDL_DisplayMode defaultMode;
        SDL_GetDesktopDisplayMode(0, &defaultMode);

        _width = defaultMode.w;
        _height = defaultMode.h;
    }

    Uint32 windowFlags = SDL_WINDOW_OPENGL;
    if (_isFullscreen) {
        LOG_INFO << "Fullscreen request." << endl;
        windowFlags |= SDL_WINDOW_FULLSCREEN;
    }

#if defined(EMSCRIPTEN)
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 0);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_ES);
#else
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 3);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
#endif
    if (_glContext) {
        SDL_GL_DeleteContext(_glContext);
        _glContext = 0;
    }

    if (_windowHandle) {
        SDL_DestroyWindow(_windowHandle);
        _windowHandle = 0;
    }

    _windowHandle = SDL_CreateWindow(GAMETITLE.c_str(), SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, _width,
        _height, windowFlags);
    if (!_windowHandle) {
        LOG_ERROR << "Video Mode: failed to create window: " << SDL_GetError() << endl;
        return false;
    }

    _glContext = SDL_GL_CreateContext(_windowHandle);
    if (!_glContext) {
        LOG_ERROR << "Video Mode: failed to create GL context: " << SDL_GetError() << endl;
        return false;
    }

    glViewport(0, 0, _width, _height);

    //reset mouse position ang grab-state
    InputS::instance()->resetMousePosition();

#if 1
    SDL_SetRelativeMouseMode(SDL_TRUE);
#else
    // SDL_SetRelativeMouseMode used to only work on Mac
    SDL_ShowCursor(SDL_DISABLE);
    bool grabMouse = true;
    ConfigS::instance()->getBoolean("grabMouse", grabMouse);
    if (grabMouse || _isFullscreen) {
        SDL_SetWindowGrab(_windowHandle, SDL_TRUE);
    }
    else {
        SDL_SetWindowGrab(_windowHandle, SDL_FALSE);
    }
    // LOG_INFO << "MOUSEX " << InputS::instance()->mousePos().x() << "\n";
    // LOG_INFO << "MOUSEY " << InputS::instance()->mousePos().y() << "\n";
    SDL_WarpMouseInWindow(_windowHandle, InputS::instance()->mousePos().x(), InputS::instance()->mousePos().y());
    SDL_Event event;
    while (SDL_PollEvent(&event)) {
        //remove any queued up events due to warping, etc.
        ;
    }
#endif

    SDL_DisplayMode currentMode;
    SDL_GetCurrentDisplayMode(0, &currentMode);
    LOG_INFO << "Video Mode: OK (" << _width << "x" << _height << "x" << SDL_BITSPERPIXEL(currentMode.format) << ")"
        << endl;

    glewInit();

    int major = -1;
    int minor = -1;
    SDL_GL_GetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, &major);
    SDL_GL_GetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, &minor);

    LOG_INFO << "OpenGL info:" << endl;
    LOG_INFO << "  Vendor  : " << glGetString(GL_VENDOR) << endl;
    LOG_INFO << "  Renderer: " << glGetString(GL_RENDERER) << endl;
    LOG_INFO << "  Version : " << glGetString(GL_VERSION) << endl;
    LOG_INFO << "  Context : " << major << "." << minor << endl;
    LOG_INFO << "  GLEW : " << glewGetString(GLEW_VERSION) << endl;

#if defined(DEBUG_OPENGL) && !defined(EMSCRIPTEN) && !defined(__APPLE__)
    glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS);
    // Not supported in webgl (es 3.0) - needs 4.3+ or ES 3.2+
    glEnable(GL_DEBUG_OUTPUT);
    glDebugMessageCallback(MessageCallback, 0);
#endif

    return true;
}

bool Video::updateSettings(void) {
    bool fullscreen = _isFullscreen;
    ConfigS::instance()->getBoolean("fullscreen", _isFullscreen);
    int width = 0;
    ConfigS::instance()->getInteger("width", width);
    int height = 0;
    ConfigS::instance()->getInteger("height", height);

    if ((fullscreen != _isFullscreen) || (width != _width) || (height != _height)) {
        reset();
        bool oldFullscreen = _isFullscreen;
        int oldWidth = _width;
        int oldHeight = _height;
        if (!setVideoMode()) {
            //new settings no good, try old
            Value* fs = new Value(oldFullscreen);
            ConfigS::instance()->updateKeyword("fullscreen", fs);

            Value* w = new Value(oldWidth);
            ConfigS::instance()->updateKeyword("width", w);

            Value* h = new Value(oldHeight);
            ConfigS::instance()->updateKeyword("height", h);

            if (!setVideoMode()) {
                //no luck, we are going down!
                SDL_QuitSubSystem(SDL_INIT_VIDEO);
                GameState::isAlive = false;
                return false;
            }
        }
        reload();
    }

    ConfigS::instance()->getBoolean("showStarfield", _showStarfield);
    ConfigS::instance()->getBoolean("showNebulas", _showNebulas);

    return true;
}

void Video::updateLogic(void) {
    _prevAngle = _angle;
    _angle += 5.0f;

    float boardWidth = (float)_board->getWidth(_boardIndex);
    if (_boardVisible && (_boardPosX < 0)) {
        _boardPosX += 9;
        if (_boardPosX > 0) {
            _boardPosX = 0;
        }
    }

    if (!_boardVisible && (_boardPosX > -boardWidth)) {
        _boardPosX -= 9;
    }
}

bool Video::update(void) {
    //    XTRACE();
    char buff[128];

    static float nextTime = Timer::getTime() + 0.5f;
    float thisTime = Timer::getTime();
    if (thisTime > nextTime) {
        if (!updateSettings()) {
            return false;
        }
        nextTime = thisTime + 0.5f;
    }

    if (_maxFPS) {
        static float sTime = Timer::getTime();
        if ((thisTime - sTime) < _fpsStepSize) {
            return true;
        }
        while ((thisTime - sTime) > _fpsStepSize) {
            sTime += _fpsStepSize;
        }
    }

    FPS::Update();

    GLBitmapFont& smallFont = *_smallFont;
    GLBitmapFont& scoreFont = *_scoreFont;
    GLBitmapFont& gameOFont = *_gameOFont;

    glEnable(GL_DEPTH_TEST);
    // glEnable(GL_LIGHTING);
    // glEnable(GL_LIGHT0);
    // glShadeModel(GL_SMOOTH);

    // glEnable(GL_NORMALIZE);
    //    glEnable( GL_RESCALE_NORMAL);

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    // glEnable(GL_COLOR_MATERIAL);

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glm::mat4& modelview = MatrixStack::model.top();
    modelview = glm::mat4(1.0);

    glm::mat4& projection = MatrixStack::projection.top();

    const float fov = 53.13f;
    projection = glm::frustum(
        (4.0 / 3.0) * (-2.0 * tan(fov * M_PI / 360.0)),  //xmin
        (4.0 / 3.0) * (2.0 * tan(fov * M_PI / 360.0)),   //xmax
        -2.0 * tan(fov * M_PI / 360.0),                  //ymin
        2.0 * tan(fov * M_PI / 360.0),                   //ymax
        2.0,                                             //znear
        1000.0);

    {
        Program* prog = ProgramManagerS::instance()->getProgram("lighting");
        prog->use();  //needed to set uniforms
        GLint projectionLoc = glGetUniformLocation(prog->id(), "projection");
        glUniformMatrix4fv(projectionLoc, 1, GL_FALSE, glm::value_ptr(projection));

        GLint viewLoc = glGetUniformLocation(prog->id(), "view");
        glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(glm::mat4(1.0)));

        GLint lightPosLoc = glGetUniformLocation(prog->id(), "lightPos");
        vec3f lightPos(20.0, 0.0, -50.0);
        glUniform3fv(lightPosLoc, 1, lightPos.array);

        GLint viewPosLoc = glGetUniformLocation(prog->id(), "viewPos");
        vec3f viewPos(0.0, 0.0, 0.0);
        glUniform3fv(viewPosLoc, 1, viewPos.array);

        GLint lightColorLoc = glGetUniformLocation(prog->id(), "lightColor");
        vec3f lightColor(1.0, 1.0, 1.0);
        glUniform3fv(lightColorLoc, 1, lightColor.array);

        GLint objectColorLoc = glGetUniformLocation(prog->id(), "objectColor");
        vec4f tileColor(1.0, 1.0, 1.0, 1.0);
        glUniform4fv(objectColorLoc, 1, tileColor.array);
    }

    CameraS::instance()->update();

    //place camera
    CameraS::instance()->place(modelview);

#if 0
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();

    const float fov = 53.13f;
    glFrustum((4.0 / 3.0) * (-2.0 * tan(53.13 * M_PI / 360.0)),  //xmin
              (4.0 / 3.0) * (2.0 * tan(53.13 * M_PI / 360.0)),   //xmax
              -2.0 * tan(53.13 * M_PI / 360.0),                  //ymin
              2.0 * tan(53.13 * M_PI / 360.0),                   //ymax
              2.0,                                               //znear
              1000.0);                                           //zfar

    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

    CameraS::instance()->update();

    //place camera
    CameraS::instance()->place();

    GLfloat light_position[] = {20.0, 0.0, -50.0, 1.0};
    glLightfv(GL_LIGHT0, GL_POSITION, light_position);
    glLightModeli(GL_LIGHT_MODEL_TWO_SIDE, GL_TRUE);
#endif

    if (GameState::isDeveloper) {
        //highlight the playing field, useful when using mouselook
        glDisable(GL_DEPTH_TEST);
        float y = tan(((float)M_PI * fov) / 360.0f) * 100;
        float x = y * 4.0f / 3.0f;

        {
            vec4f v[4] = {
                vec4f(-x, y, -100, 1),
                vec4f(x, y, -100, 1),
                vec4f(x, -y, -100, 1),
                vec4f(-x, -y, -100, 1),
            };
            GLVBO vbo;
            vbo.setColor(1.0f, 1.0f, 1.0f, 0.1f);
            vbo.DrawQuad(v);
        }
        /*
        glColor4f(1.0f, 1.0f, 1.0f, 0.1f);
        glBegin(GL_QUADS);
        glVertex3f(-x, y, -100);
        glVertex3f(x, y, -100);
        glVertex3f(x, -y, -100);
        glVertex3f(-x, -y, -100);
        glEnd();
        */
        glEnable(GL_DEPTH_TEST);
    }

    StarfieldS::instance()->draw(_showStarfield, _showNebulas);

    if (GameState::context != Context::eMenu) {
        if (HeroS::instance()->alive()) {
            HeroS::instance()->draw();
        }

        ParticleGroupManagerS::instance()->draw();
    }

    //--- Ortho stuff from here on ---
    /*
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glOrtho(-0.5, VIDEO_ORTHO_WIDTH + 0.5, -0.5, VIDEO_ORTHO_HEIGHT + 0.5, -1000.0, 1000.0);

    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    */
    // glm::mat4& projection = MatrixStack::projection.top();
    modelview = glm::mat4(1.0);
    projection = glm::ortho(-0.5f, VIDEO_ORTHO_WIDTH + 0.5f, -0.5f, VIDEO_ORTHO_HEIGHT + 0.5f, -1000.0f, 1000.0f);
    {
        Program* prog = ProgramManagerS::instance()->getProgram("lighting");
        prog->use();  //needed to set uniforms
        GLint projectionLoc = glGetUniformLocation(prog->id(), "projection");
        glUniformMatrix4fv(projectionLoc, 1, GL_FALSE, glm::value_ptr(projection));
    }

    {
        Program* prog = ProgramManagerS::instance()->getProgram("texture");
        prog->use();  //needed to set uniforms
        GLint modelViewMatrixLoc = glGetUniformLocation(prog->id(), "modelViewMatrix");
        glUniformMatrix4fv(modelViewMatrixLoc, 1, GL_FALSE, glm::value_ptr(projection * modelview));
    }

    glDisable(GL_DEPTH_TEST);
    // glDisable(GL_LIGHTING);

    bool showFPS = false;
    ConfigS::instance()->getBoolean("showFPS", showFPS);
    if (showFPS) {
        smallFont.setColor(1.0, 1.0, 1.0, 1.0);
        smallFont.DrawString(FPS::GetFPSString(), 0, 0, 0.6f, 0.6f);
    }

    if (GameState::context != Context::eMenu) {
        if (!HeroS::instance()->alive()) {
            gameOFont.setColor(1.0f, 1.0f, 1.0f, 0.8f);
            gameOFont.DrawString("GAME OVER", 80, 320, 1.3f, 1.3f);

            smallFont.setColor(1.0f, 0.0f, 0.0f, 0.8f);
            smallFont.DrawString("Too dangerous for ya? Try again!", 215, 260, 1.0f, 1.0f);

            if (ScoreKeeperS::instance()->currentIsTopTen()) {
                if (!_textInput.isOn()) {
                    _textInput.turnOn();
                }

                smallFont.setColor(1.0f, 1.0f, 1.0f, 1.0f);
                smallFont.DrawString("Top Ten Finish! RIP", 215, 200, 1.0f, 1.0f);

                string pname = "Enter Pilot name: ";
                pname += _textInput.getText() + "_";
                smallFont.setColor(1.0f, 0.852f, 0.0f, 1.0f);
                smallFont.DrawString(pname.c_str(), 215, 140, 1.0f, 1.0f);

                ScoreKeeperS::instance()->setName(_textInput.getText());
            } else {
                string moremorequickquick = "Press right mouse button to restart";
                smallFont.setColor(1.0f, 0.852f, 0.0f, 1.0f);
                smallFont.DrawString(moremorequickquick.c_str(), 215, 140, 1.0f, 1.0f);
            }
        }
    }

    if (GameState::isDeveloper) {
        static float nextShow = 0;
        static int aCount = 0;
        float thisTime = Timer::getTime();
        if (thisTime > nextShow) {
            nextShow = thisTime + 0.5f;
            aCount = ParticleGroupManagerS::instance()->getAliveCount();
        }
        sprintf(buff, "p=%d", aCount);
        smallFont.setColor(1.0f, 1.0f, 1.0f, 1.0f);
        smallFont.DrawString(buff, 0, 40, 1.0, 1.0);
    }

    if (GameState::context == Context::eMenu) {
        // glEnable(GL_TEXTURE_2D);

        float z = -1.0;
        float dx = 1.0 / 512.0;

        _titleA->bind();
        /*
        glBegin(GL_QUADS);
        glTexCoord2f(dx, dx);
        glVertex3f(350, 740, z);
        glTexCoord2f(1.0f - dx, dx);
        glVertex3f(500, 740, z);
        glTexCoord2f(1.0f - dx, 1 - dx);
        glVertex3f(500, 560, z);
        glTexCoord2f(dx, 1 - dx);
        glVertex3f(350, 560, z);
        glEnd();
        */
        {
            vec4f v[4] = {
                vec4f(350, 740, z, 1),
                vec4f(500, 740, z, 1),
                vec4f(500, 560, z, 1),
                vec4f(350, 560, z, 1),
            };
            vec2f t[4] = {
                vec2f(dx, dx),
                vec2f(1.0f - dx, dx),
                vec2f(1.0f - dx, 1 - dx),
                vec2f(dx, 1 - dx),
            };

            GLVBO vbo;
            vbo.setColor(1.0, 1.0, 1.0, 1.0);
            vbo.DrawTexQuad(v, t);
        }

        _titleB->bind();
        /*
        glBegin(GL_QUADS);
        glTexCoord2f(dx, dx);
        glVertex3f(500, 740, z);
        glTexCoord2f(1.0f - dx, dx);
        glVertex3f(650, 740, z);
        glTexCoord2f(1.0f - dx, 1 - dx);
        glVertex3f(650, 560, z);
        glTexCoord2f(dx, 1 - dx);
        glVertex3f(500, 560, z);
        glEnd();
        */

        {
            vec4f v[4] = {
                vec4f(500, 740, z, 1),
                vec4f(650, 740, z, 1),
                vec4f(650, 560, z, 1),
                vec4f(500, 560, z, 1),
            };
            vec2f t[4] = {
                vec2f(dx, dx),
                vec2f(1.0f - dx, dx),
                vec2f(1.0f - dx, 1 - dx),
                vec2f(dx, 1 - dx),
            };

            GLVBO vbo;
            vbo.setColor(1.0, 1.0, 1.0, 1.0);
            vbo.DrawTexQuad(v, t);
        }

        // glDisable(GL_TEXTURE_2D);

        MenuManagerS::instance()->draw();

        // glColor4f(1.0, 1.0, 1.0, 0.5);
        smallFont.setColor(1.0, 1.0, 1.0, 0.5);
        string gVersion = "v" + GAMEVERSION;
        float width = smallFont.GetWidth(gVersion.c_str(), 0.6f);
        smallFont.DrawString(gVersion.c_str(), 995.0f - width, 5.0f, 0.6f, 0.4f);
    } else {
        float boardWidth = (float)_board->getWidth(_boardIndex);

        //draw board if at least partially visible
        if (_boardPosX > -boardWidth) {
            float size = 0.48f;
            float tdy = -36.0f;
            float ty = (float)VIDEO_ORTHO_HEIGHT - 56.0f;
            float tx = 28.0f + _boardPosX;

            _board->bind();
            _board->setColor(1.0, 1.0, 1.0, 0.5);
            _board->Draw(_boardIndex, _boardPosX, VIDEO_ORTHO_HEIGHT - 256, 1.0, 1.0);

            scoreFont.setColor(1.0, 1.0, 1.0, 0.5);

            sprintf(buff, "%d", ScoreKeeperS::instance()->getCurrentScore());
            scoreFont.DrawString(buff, tx, ty, size, size);
            ty += tdy;

            sprintf(buff, "%d", ScoreKeeperS::instance()->getHighScore());
            scoreFont.DrawString(buff, tx, ty, size, size);
            ty += tdy;

            float he = HeroS::instance()->getEnergy();
            if (he < 0.0) {
                he = 0.0;
            }

            {
                vec4f v[4] = {
                    vec4f(tx, ty + 2, -1, 1),
                    vec4f(tx + he * .97f, ty + 2, -1, 1),
                    vec4f(tx + he * .97f, ty + 20, -1, 1),
                    vec4f(tx, ty + 20, -1, 1),
                };
                GLVBO vbo;
                vbo.setColor(1.0f, 0.1f, 0.1f, 0.5f);
                vbo.DrawQuad(v);
            }
            /*
            glColor4f(1.0f, 0.1f, 0.1f, 0.5f);
            glBegin(GL_QUADS);
            glVertex3f(tx, ty + 2, -1);
            glVertex3f(tx + he * .97f, ty + 2, -1);
            glVertex3f(tx + he * .97f, ty + 20, -1);
            glVertex3f(tx, ty + 20, -1);
            glEnd();
            */

            sprintf(buff, "%d", (int)he);
            scoreFont.setColor(1.0, 1.0, 1.0, 0.5);
            scoreFont.DrawString(buff, tx, ty, size, size);
            ty += tdy;

            float se = HeroS::instance()->getShieldEnergy();
            {
                vec4f v[4] = {
                    vec4f(tx, ty + 2, -1, 1),
                    vec4f(tx + se * .97f, ty + 2, -1, 1),
                    vec4f(tx + se * .97f, ty + 20, -1, 1),
                    vec4f(tx, ty + 20, -1, 1),
                };
                GLVBO vbo;
                vbo.setColor(1.0f, 0.8f, 0.0f, 0.5f);
                vbo.DrawQuad(v);
            }
            /*
            glColor4f(1.0f, 0.8f, 0.0f, 0.5f);
            glBegin(GL_QUADS);
            glVertex3f(tx, ty + 2, -1);
            glVertex3f(tx + se * .97f, ty + 2, -1);
            glVertex3f(tx + se * .97f, ty + 20, -1);
            glVertex3f(tx, ty + 20, -1);
            glEnd();
            */

            sprintf(buff, "%d", (int)se);
            scoreFont.setColor(1.0, 1.0, 1.0, 0.5);
            scoreFont.DrawString(buff, tx, ty, size, size);
            ty += tdy;

            float we = HeroS::instance()->getWeaponEnergy() / (HeroS::instance()->getMaxWeaponEnergy() / 100.0);
            {
                vec4f v[4] = {
                    vec4f(tx, ty + 2, -1, 1),
                    vec4f(tx + we * .97f, ty + 2, -1, 1),
                    vec4f(tx + we * .97f, ty + 20, -1, 1),
                    vec4f(tx, ty + 20, -1, 1),
                };
                GLVBO vbo;
                vbo.setColor(0.2f, 1.0f, 0.2f, 0.5f);
                vbo.DrawQuad(v);
            }
            /*
            glColor4f(0.2f, 1.0f, 0.2f, 0.5f);
            glBegin(GL_QUADS);
            glVertex3f(tx, ty + 2, -1);
            glVertex3f(tx + we * .97f, ty + 2, -1);
            glVertex3f(tx + we * .97f, ty + 20, -1);
            glVertex3f(tx, ty + 20, -1);
            glEnd();
            */

            sprintf(buff, "%d", (int)HeroS::instance()->getWeaponEnergy());
            scoreFont.setColor(1.0, 1.0, 1.0, 0.5);
            scoreFont.DrawString(buff, tx, ty, size, size);
            ty += tdy;

            SkillS::instance();
            string theSkill = Skill::getString(GameState::skill);
            scoreFont.DrawString(theSkill.c_str(), tx, ty, size, size);

            //glEnable(GL_LIGHTING);
            glEnable(GL_DEPTH_TEST);

            /*
            GLfloat light_position2[] = {820.0, 620.0, 500.0, 0.0};
            glLightfv(GL_LIGHT0, GL_POSITION, light_position2);
            glDisable(GL_TEXTURE_2D);
            */

            float iAngle = _prevAngle + (_angle - _prevAngle) * GameState::frameFraction;

            {
                MatrixStack::model.push(MatrixStack::model.top());
                glm::mat4& modelview = MatrixStack::model.top();
                modelview = glm::translate(modelview, glm::vec3(154.0 + _boardPosX, 675.0, -1.0));
                modelview = glm::rotate(modelview, glm::radians(iAngle), glm::vec3(0., 1., 0.));
                HeroS::instance()->drawWeapon(0);
                MatrixStack::model.pop();
            }

            {
                MatrixStack::model.push(MatrixStack::model.top());
                glm::mat4& modelview = MatrixStack::model.top();
                modelview = glm::translate(modelview, glm::vec3(154.0 + _boardPosX, 621.0, -1.0));
                modelview = glm::rotate(modelview, glm::radians(iAngle), glm::vec3(0., 1., 0.));
                HeroS::instance()->drawWeapon(1);
                MatrixStack::model.pop();
            }

            {
                MatrixStack::model.push(MatrixStack::model.top());
                glm::mat4& modelview = MatrixStack::model.top();
                modelview = glm::translate(modelview, glm::vec3(154.0 + _boardPosX, 567.0, -1.0));
                modelview = glm::rotate(modelview, glm::radians(iAngle), glm::vec3(0., 1., 0.));
                HeroS::instance()->drawWeapon(2);
                MatrixStack::model.pop();
            }
        }
    }

    SDL_GL_SwapWindow(_windowHandle);

    return true;
}

void Video::takeSnapshot(void) {
    static int count = 0;

    char filename[128];
    sprintf(filename, "snap%02d.png", count++);
    SDL_Surface* img;

    img = SDL_CreateRGBSurface(0, _width, _height, 24, 0x00FF0000, 0x0000FF00, 0x000000FF, 0);

    if (img) {
        glReadPixels(0, 0, _width, _height, GL_RGB, GL_UNSIGNED_BYTE, img->pixels);

        LOG_INFO << "Writing snapshot: " << filename << endl;
        if (!PNG::Snapshot(img, filename)) {
            LOG_ERROR << "Failed to save snapshot." << endl;
        }
        SDL_FreeSurface(img);
    }
    else {
        LOG_ERROR << "Failed to create surface for snapshot." << endl;
        LOG_ERROR << "SDL: " << SDL_GetError() << "\n";
    }
}
