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
#include "zStream.hpp"

#include "Trace.hpp"
#include "Endian.hpp"
#include "zStreamBufferImplLZMA.hpp"
#include "zStreamBufferImplZLib.hpp"

using namespace std;

ofstream &operator<<( ofstream &outfile, Uint32 i)
{       
    char *pVal = (char*)&i;
        
    if( ! ::isLittleEndian()) swap( pVal, 4);
    
    outfile.write( pVal, 4);
        
    return outfile;
}       

ifstream &operator>>( ifstream &infile, Uint32 &i)
{           
    char *pVal = (char*)&i;
        
    infile.read( pVal, 4);
    
    if( ! ::isLittleEndian()) swap( pVal, 4);
        
    return infile;
}       

//-----------------------------------------------------------------------------

ziStreamBuffer::ziStreamBuffer( ifstream &fd):
    _fstream( fd),
    _isOK(false),
    _impl(0),
    _startOffset( fd.tellg())
{
    XTRACE();
    _init();
}

void ziStreamBuffer::_init( void)
{
    XTRACE();

    setg( _buffer, _buffer, _buffer );

    _tellg = 0;
    if( !_fstream.is_open())
    {
	LOG_ERROR << "Input stream not open!" << endl;
    }

//    xsetflags( ios::binary);

    Uint32 magic;
    _fstream >> magic;

//    LOG_INFO << "Magic 0x" << hex << magic << dec << endl;
    if( magic == ZSTREAM_MAGIC)
    {
    	_impl = new ziStreamBufferImplZLib( _fstream);
    }
    else if( magic == LZMASTREAM_MAGIC)
    {
#ifdef USE_LZMA
    	_impl = new ziStreamBufferImplLZMA( _fstream);
#else
	LOG_ERROR << "LZMA not supported\n";
	exit(-1);
#endif
    }
    else
    {
	_fstream.seekg( -4, ios::cur);
	_impl = new ziStreamBufferImpl( _fstream);
    }

    _isOK = _impl->init();
}

ziStreamBuffer::~ziStreamBuffer()
{
    XTRACE();
    delete _impl;
}

void ziStreamBuffer::reset( void)
{
    delete _impl;
    _fstream.clear();
    _fstream.seekg( _startOffset, ios::beg);
    _init();
}

//-----------------------------------------------------------------------------

zoStreamBuffer::zoStreamBuffer( ofstream &fd, CompressionType compressionType):
    _fstream( fd),
    _isOK(false),
    _impl(0)
{
    XTRACE();

    switch( compressionType)
    {
    	case eZLibCompression:
	    _impl = new zoStreamBufferImplZLib( fd);
	    break;

    	case eLZMACompression:
#ifdef USE_LZMA
	    _impl = new zoStreamBufferImplLZMA( fd);
#else
	    LOG_ERROR << "LZMA not supported\n";
#endif
	    break;

    	case eNoCompression:
	default:
	    _impl = new zoStreamBufferImpl( fd);
	    break;
    }

    _init();
}

void zoStreamBuffer::_init( void)
{
    XTRACE();
    if( !_fstream.is_open())
    {
	LOG_ERROR << "Output stream not open!" << endl;
    }

//    xsetflags( ios::binary);

    _isOK = _impl->init();
}

zoStreamBuffer::~zoStreamBuffer()
{
    XTRACE();

    delete _impl;
}

streamsize zoStreamBuffer::xsputn(const char* s, streamsize n)
{
//    XTRACE();

    streamsize numBytes = _impl->write( s, n);

    return numBytes;
}

//-----------------------------------------------------------------------------

//Note: This rather stupid looking *new + ref for _streambuf is to avoid 
//gcc asking for -fhuge-objects when cross compiling to windows.

ziStream::ziStream( ifstream &inf, int aFileSize):
    super(0),
    _streambuf( * new ziStreamBuffer( inf)),
    _fileSize(aFileSize)
{
    XTRACE();
    init( &_streambuf);
}

ziStream::~ziStream()
{
    XTRACE();
    delete &_streambuf;
}

//-----------------------------------------------------------------------------

zoStream::zoStream( ofstream &outf, CompressionType compressionType):
    super(0),
    _streambuf( * new zoStreamBuffer( outf, compressionType))
{
    XTRACE();
    init( &_streambuf);
}

zoStream::~zoStream()
{
    XTRACE();
    delete &_streambuf;
}

//-----------------------------------------------------------------------------
