#pragma once
#ifdef USE_ONLINE_UPDATE
// Description:
//   Online update check.
//
// Copyright (C) 2005 Frank Becker
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
#include <string>

#include <Trace.hpp>
#include <Singleton.hpp>
#include "SDL/SDL_thread.h"

class OnlineUpdate
{
friend class Singleton<OnlineUpdate>;
public:
    struct NewsItem
    {
	std::string title;
	std::string date;
	std::string text;
	float r;
	float g;
	float b;
    };

    enum UpdateStatus
    {
	eNotChecking,
	eDownloading,
	eFailure,
	eSuccess,
	eLAST
    };

    void init( const std::string &defaultUpdateURL);
    void getUpdate( void);
    UpdateStatus getUpdateStatus( void) { return _status; }

    bool isLatest( void) { return _isLatest; }
    const std::string getLatestVersion( void) { return _latestVersion; }
    const std::string getLatestVersionDate( void) { return _latestVersionDate; }
    const std::string getLatestVersionText( void) { return _latestVersionText; }

    std::list<NewsItem*>& getNewsItems( void) { return _newsItemList; }

private:
    ~OnlineUpdate();
    OnlineUpdate( void);
    OnlineUpdate( const OnlineUpdate&);
    OnlineUpdate &operator=( const OnlineUpdate&);

    static int run(void *data);
    static size_t write(void *buffer, size_t size, size_t nmemb, void *data);

    bool process( void);
    void loadUpdateCache(void);
    void loadUpdateCache(const std::string &updateCache);

    std::string _updateURL;
    SDL_Thread *_thread;
    std::string _text;
    UpdateStatus _status;
    bool _isLatest;
    std::string _latestVersion;
    std::string _latestVersionDate;
    std::string _latestVersionText;
    std::list<NewsItem*> _newsItemList;
};

typedef Singleton<OnlineUpdate> OnlineUpdateS;
#endif
