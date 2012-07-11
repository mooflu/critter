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
#include <Trace.hpp>

#include "../../../Common/MyWindows.h"
#include "../../../Common/MyInitGuid.h"

#include "zStreamBufferImplLZMA.hpp"

bool ziStreamBufferImplLZMA::init( void)
{
    XTRACE();
    _decoder= new NCompress::NLZMA::CDecoder;

    const UInt32 kPropertiesSize = 5;
    Byte properties[kPropertiesSize];
    UInt32 processedSize;
    if (Read(properties, kPropertiesSize, &processedSize) != S_OK)
    {
        LOG_ERROR << "Failed to read properties\n";
	return false;
    }
    if (processedSize != kPropertiesSize)
    {
        LOG_ERROR << "Failed to read properties\n";
	return false;
    }
    if (_decoder->SetDecoderProperties2(properties, kPropertiesSize) != S_OK)
    {
        LOG_ERROR << "Unable to set decoder propertied\n";
	return false;
    }

    _decoder->SetInStream(this);
    _decoder->SetOutStreamSize(0);

    return true;
}

std::streamsize ziStreamBufferImplLZMA::read( char *s, std::streamsize n)
{
    XTRACE();
    UInt32 psize = 0;
//    LOG_INFO << "Request for  " << n << " bytes\n";
    if( _decoder->Read( s, n, &psize) != S_OK)
    {
	LOG_ERROR << "Failed to decompress...\n";
    }
//    LOG_INFO << "Uncompressed " << psize << " bytes\n";
    return psize;
#if 0
    UInt32 psize = 0;
    UInt32 total = 0;
    do
    {
        LOG_INFO << "psize = " << psize << endl;
        total += psize;
    } while (psize > 0);
    LOG_INFO << "total = " << total << endl;
#endif
}

//------------------------------------------------------------------------------

zoStreamBufferImplLZMA::~zoStreamBufferImplLZMA()
{
    XTRACE();
//    LOG_INFO << "~zoStreamBufferImplLZMA" << endl;
    UInt64 processedInSize;
    UInt64 processedOutSize;
    Int32 finished = 0;
    while( ! finished)
    {
	if( _encoder->CodeOneBlock(&processedInSize, &processedOutSize, &finished) != S_OK)
	{
	    LOG_ERROR << "Failed to compress data\n";
	    break;
	}
//	LOG_INFO << "insize=" << processedInSize << " outsize=" << processedOutSize 
//		 << ((finished==1)?" Done":" More!") << endl;
    }
    delete _encoder;
}

bool zoStreamBufferImplLZMA::init( void)
{
    XTRACE();
    _fstream << (Uint32) LZMASTREAM_MAGIC;

    _encoder= new NCompress::NLZMA::CEncoder;

    UInt32 algorithm = 2;               // max compression
    UInt32 dictionary = 1 << 23;        // 8 Meg dictionary
    bool eos = true;

    PROPID propIDs[] =
    {
	NCoderPropID::kDictionarySize,
	NCoderPropID::kAlgorithm,
	NCoderPropID::kEndMarker
    };
    const int kNumProps = sizeof(propIDs) / sizeof(propIDs[0]);
    PROPVARIANT properties[kNumProps];
    properties[0].vt = VT_UI4;
    properties[0].ulVal = UInt32(dictionary); properties[1].vt = VT_UI4;
    properties[1].ulVal = UInt32(algorithm);
    properties[2].vt = VT_BOOL;
    properties[2].boolVal = eos ? VARIANT_TRUE : VARIANT_FALSE;

    if (_encoder->SetCoderProperties(propIDs, properties, kNumProps) != S_OK)
    {
        LOG_ERROR << "Unable to set properties\n";
	return false;
    }

    _encoder->WriteCoderProperties(this);
    _encoder->SetStreams( this, this, 0, 0);

    return true;
}

std::streamsize zoStreamBufferImplLZMA::write( const char *s, std::streamsize n)
{
    XTRACE();
//    LOG_INFO << "Write request " << n << endl;
    _currentBuffer = s;
    _bufferSize = n;
    UInt64 processedInSize;
    UInt64 processedOutSize;
    Int32 finished = 0;
    while( _currentBuffer)
    {
	if( _encoder->CodeOneBlock(&processedInSize, &processedOutSize, &finished) != S_OK)
	{
	    LOG_ERROR << "Failed to compress data\n";
	    return 0;
	}
//	LOG_INFO << "insize=" << processedInSize << " outsize=" << processedOutSize 
//		 << ((finished==1)?" Done":" More!") << endl;
    }

    return n;

#if 0
    UInt64 processedInSize;
    UInt64 processedOutSize;
    Int32 finished = 0;
    while( ! finished)
    {
        if( _encoder->CodeOneBlock(&processedInSize, &processedOutSize, &finished) != S_OK)
        {
            LOG_ERROR << "Failed to compress data\n";
	    break;
        }
        LOG_INFO << "insize = " << processedInSize << " outsize = " << processedOutSize << endl;
    }
#endif
}
#endif
