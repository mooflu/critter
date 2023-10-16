#pragma once
// Description:
//   gettimeofday wrapper
//
// Copyright (C) 2003 Frank Becker
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

#ifdef _MSC_VER
#include <Winsock2.h>
#include <sys/timeb.h>

struct timezone {
    int tz_minuteswest;
    int tz_dsttime;
};

static inline int gettimeofday(struct timeval* tv, struct timezone* /*tz*/) {
    struct timeb t;
    ftime(&t);

    tv->tv_sec = (long)t.time;
    tv->tv_usec = t.millitm * 1000;

    return 0;
}
#else
#include <sys/time.h>
#endif
