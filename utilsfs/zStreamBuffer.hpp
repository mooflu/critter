#pragma once
// Description:
//   A compressed (file) input/output stream.
//
// Copyright (C) 2011 Frank Becker
//
#include <string>
#include <streambuf>

extern "C" {
struct PHYSFS_File;
}

class ziStreamBuffer : public std::streambuf {
    typedef int int_type;

public:
    ziStreamBuffer(const std::string& fileName);
    ~ziStreamBuffer();

    bool isOK(void) { return _isOK; }

    int fileSize(void);

    void* getHandle(void) { return _physFile; }

protected:
    virtual int_type underflow(void);
    virtual pos_type seekoff(off_type pos, std::ios_base::seekdir, std::ios_base::openmode);
    virtual pos_type seekpos(pos_type pos, std::ios_base::openmode);

private:
    static const int kReadBufferSize = 1024;
    PHYSFS_File* _physFile;
    char _buf[kReadBufferSize];

    bool _init(const std::string& fileName);
    bool _isOK;

private:
    ziStreamBuffer(void);
    ziStreamBuffer(const ziStreamBuffer&);
    ziStreamBuffer& operator=(const ziStreamBuffer&);
};

class zoStreamBuffer : public std::streambuf {
public:
    zoStreamBuffer(const std::string& filename);
    ~zoStreamBuffer();

    bool isOK(void) { return _isOK; }

    void* getHandle(void) { return _physFile; }

protected:
    virtual int overflow(int c);
    virtual int sync(void);

private:
    static const int kWriteBufferSize = 1024;
    PHYSFS_File* _physFile;
    char _buf[kWriteBufferSize];

    bool _init(const std::string& fileName);
    bool _isOK;

private:
    zoStreamBuffer(void);
    zoStreamBuffer(const zoStreamBuffer&);
    zoStreamBuffer& operator=(const zoStreamBuffer&);
};
