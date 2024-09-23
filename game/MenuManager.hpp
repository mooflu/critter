#pragma once
// Description:
//   Menu Manager/Controller.
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

#include <list>

#include "tinyxml.h"
#include "Singleton.hpp"
#include "InterceptorI.hpp"
#include "Context.hpp"
#include "Model.hpp"
#include "ParticleGroup.hpp"

class Selectable;
class GLTextureCubeMap;

class MenuManager : public InterceptorI {
    friend class Singleton<MenuManager>;

public:
    bool init(void);
    bool update(void);
    bool draw(void);

    virtual void input(const Trigger& trigger, const bool& isDown);
    void turnMenuOn(void);
    void turnMenuOff(void);
    bool canReturnToGame(void);
    void makeMenu(TiXmlNode* _node);

    //Menu navigation
    void Down(void);
    void Up(void);
    void Enter(void);
    bool Exit(bool delayed = false);
    void Goto(Selectable* s);

    void reset(void);
    void reload(void);

private:
    virtual ~MenuManager();
    MenuManager(void);
    MenuManager(const MenuManager&);
    MenuManager& operator=(const MenuManager&);

    void loadMenuLevel(void);
    void clearActiveSelectables(void);
    void updateSettings(void);
    void activateSelectableUnderMouse(const bool& useFallback = false);
    void updateMousePosition(const Trigger& trigger);

    TiXmlDocument* _menu;

    TiXmlNode* _topMenu;
    TiXmlNode* _currentMenu;

    std::list<Selectable*> _activeSelectables;
    std::list<Selectable*>::iterator _currentSelectable;

    int _board;
    Point2Di _boardOffset;

    int _pointer;

    float _prevMouseX;
    float _prevMouseY;
    float _mouseX;
    float _mouseY;

    Context::ContextEnum _prevContext;
    bool _delayedExit;
    bool _newLevelLoaded;

    Model* _nextGenShippy;
    Model* _mapleLeaf;
    float _angle;
    float _prevAngle;

    bool _showSparks;
    ParticleGroup _burst;
    GLTextureCubeMap* _nextGenShippyCubeMap;
};

typedef Singleton<MenuManager> MenuManagerS;
