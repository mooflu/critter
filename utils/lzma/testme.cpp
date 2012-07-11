#include <fstream>
#include <iostream>
using namespace std;

#include "StdAfx.h"

#include "../../../Common/MyWindows.h"
#include "../../../Common/MyInitGuid.h"

#include "LZMADecoder.h"
#include "LZMAEncoder.h"

class MyInStream: public ISequentialInStream, public CMyUnknownImp
{
public:
    MyInStream( char *filename):
    	_fstream( filename, ios::in | ios::binary)
    {

    }
    virtual ~MyInStream(){}

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

    LONG QueryInterface(const GUID&, void**){ return 0;}
    ULONG AddRef(){ return 0;}
    ULONG Release(){ return 0;}

private:
    ifstream _fstream;
};

class MyOutStream: public ISequentialOutStream, public CMyUnknownImp
{
public:
    MyOutStream( char *filename):
    	_fstream( filename, ios::out | ios::binary)
    {
    }
    virtual ~MyOutStream(){}

    STDMETHOD(Write)(const void *data, UInt32 size, UInt32 *processedSize)
    {
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

    LONG QueryInterface(const GUID&, void**){ return 0;}
    ULONG AddRef(){ return 0;}
    ULONG Release(){ return 0;}

private:
    ofstream _fstream;
};

class MyCompressProgressInfo: public ICompressProgressInfo
{
public:
    MyCompressProgressInfo( void)
    {
    }

    STDMETHOD(SetRatioInfo)(const UInt64 *inSize, const UInt64 *outSize)
    {
	cout << "insize  = " << *inSize << endl;
	cout << "outsize = " << *outSize << endl;
	return S_OK;
    }

    LONG QueryInterface(const GUID&, void**){ return 0;}
    ULONG AddRef(){ return 0;}
    ULONG Release(){ return 0;}
};

void Compress( void)
{
    MyInStream *inStream = new MyInStream( "test.in");

    NCompress::NLZMA::CEncoder *encoder= new NCompress::NLZMA::CEncoder;

    UInt32 algorithm = 2;		// max compression
    UInt32 dictionary = 1 << 23;	// 8 Meg dictionary
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

    if (encoder->SetCoderProperties(propIDs, properties, kNumProps) != S_OK)
    {
	cout << "Unable to set properties\n";
	exit(-1);
    }

    MyOutStream *outStream = new MyOutStream( "test.out");
    encoder->WriteCoderProperties(outStream);
    encoder->SetStreams( inStream, outStream, 0, 0);

    UInt64 processedInSize;
    UInt64 processedOutSize;
    Int32 finished = 0;
    while( ! finished)
    {
	if( encoder->CodeOneBlock(&processedInSize, &processedOutSize, &finished) != S_OK)
	{
	    cout << "Failed to compress data\n";
	    exit(-1);
	}
	cout << "insize = " << processedInSize << " outsize = " << processedOutSize << endl;
    }

#if 0
    if( encoder->Code(inStream, outStream, 0, 0, 0) != S_OK)
    {
	cout << "Failed to compress data\n";
	exit(-1);
    }
#endif
}

void Decompress( void)
{
    MyInStream *inStream = new MyInStream( "test.out");

    NCompress::NLZMA::CDecoder *decoder= new NCompress::NLZMA::CDecoder;

    const UInt32 kPropertiesSize = 5;
    Byte properties[kPropertiesSize];
    UInt32 processedSize;
    if (inStream->Read(properties, kPropertiesSize, &processedSize) != S_OK)
    {
	cout << "Failed to read properties\n";
	exit(-1);
    }
    if (processedSize != kPropertiesSize)
    {
	cout << "Failed to read properties\n";
	exit(-1);
    }
    if (decoder->SetDecoderProperties2(properties, kPropertiesSize) != S_OK)
    {
	cout << "Unable to set decoder propertied\n";
	exit(-1);
    }

    decoder->SetInStream(inStream);
    decoder->SetOutStreamSize(0);
    char buffer[2048];
    UInt32 psize = 0;
    UInt32 total = 0;
    do
    {
	decoder->Read( buffer, 2048, &psize);
	cout << "psize = " << psize << endl;
	total += psize;
    } while (psize > 0);
    cout << "total = " << total << endl;

#if 0
    MyCompressProgressInfo *progress = new MyCompressProgressInfo;
    MyOutStream *outStream = new MyOutStream( "test.out2");

    if (decoder->Code(inStream, outStream, 0, 0, progress) != S_OK)
    {
	cout << "Failed to decompress data\n";
	exit(-1);
    }
#endif
}

int main( int argc, char *argv[])
{
    Compress();
    Decompress();

    return 0;
}
 
