#pragma once
// Description:
//   Base StreamBuffer impl
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

#include <fstream>

//make sure Uint32 are written as binary
typedef unsigned int Uint32;
std::ofstream &operator<<( std::ofstream &outfile, Uint32 i);
std::ifstream &operator>>( std::ifstream &infile, Uint32 &i);

const Uint32 ZSTREAM_MAGIC = 0xabcdef55;
const Uint32 LZMASTREAM_MAGIC = 0xabcdef56;

class ziStreamBufferImpl
{
public:
    ziStreamBufferImpl( std::ifstream &ifs):
	_fstream( ifs),
	_eof(false)
    {
    }

    virtual ~ziStreamBufferImpl()
    {
    }

    virtual bool init( void)
    {
    	return true;
    }

    virtual std::streamsize read( char* s, std::streamsize n)
    {
	if( _fstream.eof()) _eof = true;
	_fstream.read( s, n);
	return  _fstream.gcount();
    }

    virtual bool isEOF( void)
    {
	return _eof;
    }

protected:
    std::ifstream &_fstream;
    bool _eof;
};

class zoStreamBufferImpl
{
public:
    zoStreamBufferImpl( std::ofstream &ofs):
	_fstream( ofs)
    {
    }

    virtual ~zoStreamBufferImpl()
    {
    }

    virtual bool init( void)
    {
    	return true;
    }

    virtual std::streamsize write( const char *s, std::streamsize n)
    {
    	_fstream.write( s, n);
	return n;
    }

protected:
    std::ofstream &_fstream;
};
