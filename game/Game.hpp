#pragma once
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

#include <Singleton.hpp>

class Game {
    friend class Singleton<Game>;

public:
    bool init(void);
    void run(void);
    void reset(void);
    void startNewGame(void);

    static void gameLoop(void);

private:
    ~Game();
    Game(void);
    Game(const Game&);
    Game& operator=(const Game&);

    void updateOtherLogic(void);
    void updateInGameLogic(void);
};

typedef Singleton<Game> GameS;
