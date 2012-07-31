// Description:
//   Limits the rate of a loop
//
// Copyright (C) 2012 Adam Winsor
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
#include <Timer.hpp>
#include <RateLimiter.hpp>

#define DEFAULT_RATE_LIMIT (100.0)

RateLimiter::RateLimiter():
    RateLimiter(DEFAULT_RATE_LIMIT)
{
    XTRACE();
}

RateLimiter::RateLimiter(int rate):
    _timeSlice(1.0/rate),
    _sliceStartTime(0.0)
{
    XTRACE();
}

RateLimiter::~RateLimiter()
{
    XTRACE();
}

void RateLimiter::reset(float currTime)
{
    this->_sliceStartTime = currTime;
}

void RateLimiter::limit(float currTime)
{
    if(!this->_enabled)
    {
        return;
    }
  
    float currentDifference = currTime - this->_sliceStartTime;
  
    if((this->_timeSlice - currentDifference) > 0.0)
    {
        Timer::sleep(this->_timeSlice - currentDifference);
        this->_sliceStartTime += this->_timeSlice;
    }
    else
    {
        //we are behind so use the currTime as the start time and don't delay
        this->_sliceStartTime = currTime;
    }
}
