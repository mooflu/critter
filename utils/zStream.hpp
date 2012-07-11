#pragma once
// Description:
//   A compressed (file) input/output stream.
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
#include <stdio.h>

#include <fstream>
#include <iostream>

#include <Trace.hpp>
#include <zStreamBufferImpl.hpp>

inline void swap( void *buf, int len)
{
    char *b=(char*)buf;
    for( int i=0; i<(len>>1); i++)
    {
	char t = b[ i];
	b[ i] = b[len-i-1]; 
	b[ len-i-1] = t;
    }
}

enum CompressionType
{
    eNoCompression,
    eZLibCompression,
    eLZMACompression
};

class ziStreamBuffer: public std::streambuf
{
    typedef std::streambuf super;
    typedef int int_type;
    typedef std::streamsize streamsize;
/*
    typedef super::int_type int_type;
    typedef std::char_traits<char> traits;
*/
    static const int kReadBufferSize = 8;
    char _buffer[kReadBufferSize];

public:
    ziStreamBuffer( std::ifstream &fd);
    virtual ~ziStreamBuffer();

    void reset( void);
    streamsize currentPos( void){ return _tellg;}
    bool isOK( void){ return _isOK;}

protected:
    virtual super* setbuf(char*, streamsize)
    {
        return this;
    }

    //underflow does NOT advance stream pointer
    virtual int_type underflow () 
    {
        if( gptr() < egptr()) 
	{
	    //return from buffer
            return traits_type::to_int_type(*gptr());
        }

        int num = _impl->read(_buffer, kReadBufferSize);
        if (num <= 0) 
	{
            return EOF;
        }

        setg( _buffer, _buffer, _buffer+num);

        return traits_type::to_int_type(*gptr());
    }

private:
    ziStreamBuffer( void);
    ziStreamBuffer( const ziStreamBuffer&);
    ziStreamBuffer &operator=(const ziStreamBuffer&);

    void _init( void);
    
    std::ifstream &_fstream;
    bool _isOK;
    ziStreamBufferImpl *_impl;
    std::streamsize _startOffset;
    std::streamsize _tellg;
};

class zoStreamBufferImpl;
class zoStreamBuffer: public std::streambuf
{
    typedef std::streambuf super;
    typedef int int_type;
    typedef std::streamsize streamsize;
/*
    typedef super::int_type int_type;
    typedef std::char_traits<char> traits;
*/
public:
    zoStreamBuffer( std::ofstream &fd, CompressionType compressionType);
    virtual ~zoStreamBuffer();
    bool isOK( void){ return _isOK;}

protected:
    virtual super* setbuf(char*, streamsize)
    {
        return this;
    }
    virtual int_type overflow( int_type c=EOF)
    {
        if( c != EOF)
        {
            char ch = c;
            streamsize n = xsputn( &ch, 1);
            if( n != 1) return EOF;
        }
        return c;
    }

    virtual streamsize xsputn(const char* s, streamsize n);

private:
    zoStreamBuffer( void);
    zoStreamBuffer( const zoStreamBuffer&);
    zoStreamBuffer &operator=(const zoStreamBuffer&);

    void _flush( void);
    void _init( void);
    
    std::ofstream &_fstream;
    bool _isOK;
    zoStreamBufferImpl *_impl;
};

class ziStream: public std::istream
{
    typedef std::istream super;

public:
    ziStream( std::ifstream &inf, int fileSize=-1);
    virtual ~ziStream();

    bool isOK( void){ return _streambuf.isOK();}
    void reset( void){ clear(); _streambuf.reset();}
    int fileSize( void){ return _fileSize; }
    std::streamsize currentPos( void){ return _streambuf.currentPos();}

private:
    ziStream( void);
    ziStream( const ziStream&);
    ziStream &operator=(const ziStream&);

    ziStreamBuffer &_streambuf;
    int _fileSize;
};

class zoStream: public std::ostream
{
    typedef std::ostream super;
    
public:
    zoStream( std::ofstream &outf, CompressionType compressionType = eZLibCompression);
    virtual ~zoStream();

    bool isOK( void){ return _streambuf.isOK();}
    
private:
    zoStream( void);
    zoStream( const zoStream&);
    zoStream &operator=(const zoStream&);

    zoStreamBuffer &_streambuf;
};  
