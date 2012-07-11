// Description:
//   SDL_RWops wrapper for compressed stream.
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
#include "zrwops.hpp"

#include <stdlib.h>
#include "Trace.hpp"

int ziStream_seek(struct SDL_RWops *context,int offset,int whence)
{
    ziStream &zi = *((ziStream*)context->hidden.unknown.data1);
    int pos = zi.currentPos();
    int delta = 0;
#if 0
    LOG_INFO << "ziStream_seek pos=" << pos 
        << " whence=" << whence
        << " offset=" << offset
	<< endl;
#endif

    switch( whence)
    {
	case SEEK_SET:
	    //LOG_INFO << "ziStream_seek SEEK_SET\n";
	    delta = offset - pos;
	    break;

	case SEEK_END:
	    //LOG_INFO << "ziStream_seek SEEK_END\n";
	    if( zi.fileSize() < 0)
	    {
		LOG_ERROR << "Cannot seek from END in this stream.\n"; 
		return -1;
	    }
	    //LOG_INFO << "ziStream_seek END size=" << zi.fileSize() << " offset=" << offset << endl;
	    delta = zi.fileSize() - offset - pos; // -1;
	    break;

	case SEEK_CUR:
	    //LOG_INFO << "ziStream_seek SEEK_CUR\n";
	    delta = offset;
	    break;

	default:
	    //LOG_INFO << "ziStream_seek unknown whence value!" << endl;
	    return -1;
	    break;
    }

    if( delta < 0)
    {
	int fromStart = pos + delta;
	if( fromStart < 0) fromStart = 0;

	//reset to beginning of file
	zi.reset();
	//LOG_INFO << "ziStream_seek reset\n";

	delta = fromStart;
    }

    if( delta > 0)
    {
	//LOG_INFO << "ziStream_seek read " << delta << "\n";
	char *dumpBuffer = new char[ delta];
	zi.read( dumpBuffer, delta);
	delete [] dumpBuffer;
	//LOG_INFO << "ziStream_seek new pos = " << zi.currentPos() << "\n";
    }

    return zi.currentPos();
}
int ziStream_read(struct SDL_RWops *context, void *ptr, int size, int maxnum)
{
    ziStream &zi = *((ziStream*)context->hidden.unknown.data1);
    zi.read( (char*)ptr, size*maxnum);
    return( maxnum);
}
int ziStream_write(struct SDL_RWops *,const void *,int,int)
{
    LOG_ERROR << "ziStream_write not implemented!\n";
    return -1;
}
int ziStream_close(struct SDL_RWops *context)
{
    if( context) 
    {
	SDL_FreeRW( context);
    }
    return(0);
}

SDL_RWops *RWops_from_ziStream( ziStream &zi)
{
    SDL_RWops *rwops;

    rwops = SDL_AllocRW();
    if ( rwops != NULL ) {
	rwops->seek = ziStream_seek;
	rwops->read = ziStream_read;
	rwops->write = ziStream_write;
	rwops->close = ziStream_close;
	rwops->hidden.unknown.data1 = &zi;
    }
    return( rwops);
}

