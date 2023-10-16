// Description:
//   A compressed (file) input/output stream.
//
// Copyright (C) 2011 Frank Becker
//
#include "zStream.hpp"
#include "zStreamBuffer.hpp"

#include <string>

ziStream::ziStream(const std::string& fileName) :
    std::istream(0),
    _streambuf(new ziStreamBuffer(fileName)) {
    rdbuf(_streambuf);
}

ziStream::~ziStream() {
    delete _streambuf;
}

bool ziStream::isOK(void) {
    return _streambuf->isOK();
}

int ziStream::fileSize(void) {
    return _streambuf->fileSize();
}

zoStream::zoStream(const std::string& fileName) :
    std::ostream(0),
    _streambuf(new zoStreamBuffer(fileName)) {
    rdbuf(_streambuf);
}

zoStream::~zoStream() {
    delete _streambuf;
}

bool zoStream::isOK(void) {
    return _streambuf->isOK();
}
