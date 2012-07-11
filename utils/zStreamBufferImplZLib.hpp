#pragma once
// Description:
//   ZLib StreamBuffer impl
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

#include "zStreamBufferImpl.hpp"
#include <zlib.h>

const int ZSTREAM_BUFFSIZE=2048;

class ziStreamBufferImplZLib: public ziStreamBufferImpl
{
public:
    ziStreamBufferImplZLib( std::ifstream &ifs):
    	ziStreamBufferImpl(ifs)
    {
    }

    virtual ~ziStreamBufferImplZLib()
    {
        inflateEnd( &_stream);
    }

    virtual bool init( void);
    virtual std::streamsize read( char* s, std::streamsize n);

private:
    Byte _inBuf[ ZSTREAM_BUFFSIZE];
    z_stream _stream;
};

//-----------------------------------------------------------------------------

class zoStreamBufferImplZLib: public zoStreamBufferImpl
{
public:
    zoStreamBufferImplZLib( std::ofstream &ofs):
    	zoStreamBufferImpl(ofs)
    {
    }

    virtual ~zoStreamBufferImplZLib();

    virtual bool init( void);
    virtual std::streamsize write( const char *s, std::streamsize n);

private:
    void _flush( void);

    Byte _outBuf[ ZSTREAM_BUFFSIZE];
    z_stream _stream;
};
