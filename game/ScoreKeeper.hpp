#pragma once
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

#include <string>
#include <vector>

#include <time.h>

#include "Point.hpp"
#include "Singleton.hpp"

using namespace std;

struct ScoreData {
    int score;
    string name;
    time_t time;
    int goodiesCaught;
    int goodiesMissed;
    int skill;
};

inline bool operator<(const ScoreData& s1, const ScoreData& s2) {
    // ">" since we want highscore first
    return s1.score > s2.score;
}

class ScoreKeeper {
    friend class Singleton<ScoreKeeper>;

public:
    ~ScoreKeeper();
    ScoreKeeper(void);

    int getCurrentScore(void) { return _leaderBoard[_currentIndex].score; }

    void resetCurrentScore(void);

    int addToCurrentScore(int value);

    void incGoodiesCaught(void);
    void incGoodiesMissed(void);

    int goodiesCaught(void);

    int getScore(unsigned int index) {
        if (index < _leaderBoard.size()) {
            return _leaderBoard[index].score;
        }

        return 0;
    }

    const string getInfoText(unsigned int index);

    int getHighScore(void) { return _leaderBoard[0].score; }

    int boardSize(void) { return (int)_leaderBoard.size(); }

    bool currentIsTopTen(void) { return _currentIndex < 10; }

    void setName(const string& name) { _leaderBoard[_currentIndex].name = name; }

    void load(void);
    void save(void);
    void draw(const Point2Di& offset);

private:
    ScoreKeeper(const ScoreKeeper&);
    ScoreKeeper& operator=(const ScoreKeeper&);

    void updateLeaderBoard(void);
    void dump(void);

    unsigned int _currentIndex;
    vector<ScoreData> _leaderBoard;
};

typedef Singleton<ScoreKeeper> ScoreKeeperS;
