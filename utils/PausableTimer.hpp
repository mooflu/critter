#pragma once
// Description:
//   Similar to Timer, but allows pausing.
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
#include "Timer.hpp"

class PausableTimer {
public:
    PausableTimer(void) :
        _isPaused(false),
        _adjust(0.0),
        _pausedAt(0.0) {
        reset();
    }

    void reset(void) {
        _isPaused = false;
        _adjust = Timer::getTime();
    }

    void pause(void) {
        if (!_isPaused) {
            _isPaused = true;
            _pausedAt = Timer::getTime() - _adjust;
        }
    }

    void start(void) {
        if (_isPaused) {
            _isPaused = false;
            _adjust = Timer::getTime() - _pausedAt;
        }
    }

    double getTime(void) {
        if (_isPaused) {
            return (_pausedAt);
        } else {
            return (Timer::getTime() - _adjust);
        }
    }

private:
    bool _isPaused;
    double _adjust;
    double _pausedAt;
};
