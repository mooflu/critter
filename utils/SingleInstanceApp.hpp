#pragma once
// Description:
//    Helper to check if an instance of an application is already running.
//    This can be on a per user basis or global depending on where the
//    unix domain socket lives.
//
// Copyright (C) 2004 Frank Becker
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

#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/un.h>

#include "Trace.hpp"

class SingleInstanceApp
{
public:
    SingleInstanceApp( string socketName):
        _socketName(socketName)
    {
	_socket = socket(AF_UNIX, SOCK_DGRAM, 0);
    }

    ~SingleInstanceApp()
    {
	unlink( _socketName.c_str());

	if( _socket != -1)
	{
	    close(_socket);
	}
    }

    bool isRunning( void)
    {
	return sendMessage( "Hello?");
    }

    bool bind( void)
    {
	struct sockaddr_un addr;
	addr.sun_family = AF_UNIX;
	strcpy(addr.sun_path, _socketName.c_str());

	unlink( addr.sun_path);
	int result = ::bind(_socket, (struct sockaddr *) &addr, strlen(addr.sun_path) + sizeof (addr.sun_family));

	if( result == -1)
	{
	    LOG_ERROR << "Failed to bind to " << _socketName << "\n";
	    perror( "Bind ");
	    return false;
	}

	return true;
    }

    int getSocket( void)
    {
	return _socket;
    }

    void receiveMessage( string &msg)
    {
	struct sockaddr_un addr;
	addr.sun_family = AF_UNIX;
	strcpy(addr.sun_path, _socketName.c_str());

	char buf[128];
	socklen_t len = strlen(addr.sun_path) + sizeof (addr.sun_family);
	recvfrom( _socket, buf, 128, 0, (struct sockaddr *) &addr, &len);

	msg = buf;
    }

    bool sendMessage( const string &msg)
    {
	struct sockaddr_un addr;
	addr.sun_family = AF_UNIX;
	strcpy(addr.sun_path, _socketName.c_str());

	socklen_t len = strlen(addr.sun_path) + sizeof (addr.sun_family);

	ssize_t count = sendto( _socket, msg.c_str(), strlen(msg.c_str())+1, 0, (struct sockaddr *) &addr, len);

	if( count == -1)
	    return false;
	else
	    return true;
    }

private:
    string _socketName;
    int _socket;
};
