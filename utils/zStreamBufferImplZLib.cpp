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
#include "Trace.hpp"
#include "zStreamBufferImplZLib.hpp"

using namespace std;

bool ziStreamBufferImplZLib::init( void)
{
    _stream.zalloc = (alloc_func)0;
    _stream.zfree = (free_func)0;
    _stream.opaque = (voidpf)0;

    _stream.next_in = _inBuf;
    _stream.avail_in = 0;

    if( inflateInit( &_stream) != Z_OK)
    {
	LOG_ERROR << "Unable to initialize ziStreamBuffer" << endl;
	return false;
    }
    return true;
}

streamsize ziStreamBufferImplZLib::read( char* s, streamsize n)
{
    _stream.next_out = (Byte*)s;
    _stream.avail_out = (uInt)n;

    do
    {
	if( _stream.avail_in == 0)
	{
	    _fstream.read( (char*)_inBuf, ZSTREAM_BUFFSIZE);
	    _stream.next_in = _inBuf;
	    _stream.avail_in = (uInt)_fstream.gcount();
	}
	int err = inflate(&_stream, Z_NO_FLUSH);
	if( err == Z_STREAM_END) _eof = true;
        else if( err != Z_OK)
        {
	    LOG_ERROR << "ziStreamBuffer::xsgetn error during inflate: " 
                      << err << endl;
            exit(-1);
        }
    }
    while( (_stream.avail_out > 0) && !_eof);

    return n-_stream.avail_out;
}

//-----------------------------------------------------------------------------

zoStreamBufferImplZLib::~zoStreamBufferImplZLib()
{
    int zerr;
    do
    {
	zerr = deflate(&_stream, Z_FINISH);
	_flush();
    }
    while( zerr == Z_OK);

    //      LOG_DEBUG << _stream.total_in << " IN bytes." << endl;
    //      LOG_DEBUG << _stream.total_out << " OUT bytes." << endl;

    if( deflateEnd( &_stream) != Z_OK)
    {
	LOG_ERROR << "Problem destroying zoStreamBuffer." << endl;
    }
}

bool zoStreamBufferImplZLib::init( void)
{
    _fstream << (Uint32) ZSTREAM_MAGIC;

    _stream.zalloc = (alloc_func)0;
    _stream.zfree = (free_func)0;
    _stream.opaque = (voidpf)0;

    _stream.next_out = _outBuf;
    _stream.avail_out = ZSTREAM_BUFFSIZE;

    if( deflateInit( &_stream, Z_BEST_COMPRESSION) != Z_OK)
    {
	LOG_ERROR << "Unable to initialize zoStreamBuffer" << endl;
	return false;
    }
    return true;
}

void zoStreamBufferImplZLib::_flush( void)
{
    XTRACE();
    int len = ZSTREAM_BUFFSIZE - _stream.avail_out;
    if( len)
    {
	_fstream.write( (char*)_outBuf, len);
	_stream.next_out = _outBuf;
	_stream.avail_out = ZSTREAM_BUFFSIZE;
    }
}

streamsize zoStreamBufferImplZLib::write( const char *s, streamsize n)
{
    _stream.next_in = (Byte*)s;
    _stream.avail_in = (uInt)n;

    while( _stream.avail_in != 0)
    {
	if( deflate(&_stream, Z_NO_FLUSH) != Z_OK)
	{
	    LOG_ERROR << "Compression error!" << endl;
	    break;
	}
	if( _stream.avail_out == 0)
	{
	    _flush();
	}
    }
    return n-_stream.avail_in;
}

//-----------------------------------------------------------------------------
