#pragma once
// Description:
//   LZMA StreamBuffer impl
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
#ifdef USE_LZMA

#include "Trace.hpp"
#include "zStreamBufferImpl.hpp"

#include <fstream>
#include <iostream>

#include "StdAfx.h"

#include "LZMADecoder.h"
#include "LZMAEncoder.h"

class ziStreamBufferImplLZMA: public ziStreamBufferImpl, 
                              public ISequentialInStream, 
			      public CMyUnknownImp
{
public:
    ziStreamBufferImplLZMA( std::ifstream &ifs):
    	ziStreamBufferImpl(ifs),
	_decoder(0)
    {
	XTRACE();
    }

    virtual ~ziStreamBufferImplLZMA()
    {
	XTRACE();
	delete _decoder;
    }

    virtual bool init( void);
    virtual std::streamsize read( char* s, std::streamsize n);

    //ISequentialInStream
    STDMETHOD(Read)(void *data, UInt32 size, UInt32 *processedSize)
    {
        if( ! _fstream.is_open()) return E_FAIL;

        _fstream.read( (char *)data, size);

        if(processedSize != NULL)
            *processedSize = _fstream.gcount();

        return S_OK;
    }

    STDMETHOD(ReadPart)(void *data, UInt32 size, UInt32 *processedSize)
    {
        return Read(data, size, processedSize);
    }

    //CMyUnknownImp
    LONG QueryInterface(const GUID&, void**){ return 0;}
    ULONG AddRef(){ return 0;}
    ULONG Release(){ return 0;}

private:
    NCompress::NLZMA::CDecoder *_decoder;
};

class zoStreamBufferImplLZMA: public zoStreamBufferImpl,
			      public ISequentialOutStream, 
                              public ISequentialInStream, 
			      public CMyUnknownImp
{
public:
    zoStreamBufferImplLZMA( std::ofstream &ofs):
    	zoStreamBufferImpl(ofs),
	_encoder(0),
	_currentBuffer(0)
    {
	XTRACE();
    }

    virtual ~zoStreamBufferImplLZMA();

    virtual bool init( void);
    virtual std::streamsize write( const char *s, std::streamsize n);

    //ISequentialOutStream
    STDMETHOD(Write)(const void *data, UInt32 size, UInt32 *processedSize)
    {
//	LOG_INFO << "===> Write " << size << " bytes\n";
        if( ! _fstream.is_open()) return E_FAIL;

        _fstream.write( (char*)data, size);

        if(processedSize != NULL)
            *processedSize = size;

        return S_OK;
    }

    STDMETHOD(WritePart)(const void *data, UInt32 size, UInt32 *processedSize)
    {
        return Write(data, size, processedSize);
    }

    //ISequentialInStream
    STDMETHOD(Read)(void *data, UInt32 size, UInt32 *processedSize)
    {
	UInt32 transferSize = min(size,_bufferSize);
//	LOG_INFO << "Transfersize " << transferSize << " bytes\n";
	if( size < _bufferSize)
	    LOG_ERROR << "FIXME: _buffersize > size!\n";
	if( _currentBuffer)
	{
//	    LOG_INFO << "Encoder wants " << size << " bytes\n";
//	    LOG_INFO << "Encoder gets " << transferSize << " bytes\n";
	    if( transferSize)
		memcpy( data, _currentBuffer, transferSize);
	    _bufferSize -= transferSize;
	    if( _bufferSize == 0) 
		_currentBuffer = 0;
	}
	if( processedSize)
	    *processedSize = transferSize;
        return S_OK;
    }

    STDMETHOD(ReadPart)(void *data, UInt32 size, UInt32 *processedSize)
    {
        return Read(data, size, processedSize);
    }

    //CMyUnknownImp
    LONG QueryInterface(const GUID&, void**){ return 0;}
    ULONG AddRef(){ return 0;}
    ULONG Release(){ return 0;}

private:
    NCompress::NLZMA::CEncoder *_encoder;
    const char *_currentBuffer;
    UInt32 _bufferSize;

};

#endif
