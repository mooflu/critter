#pragma once
// Description:
//   A compressed (file) input/output stream.
//
// Copyright (C) 2011 Frank Becker
//
#include <iostream>
#include <sstream>

class ziStreamBuffer;
class zoStreamBuffer;

class ziStream : public std::istream {
public:
    ziStream(const std::string& fileName);
    ~ziStream();

    bool isOK(void);
    int fileSize(void);

    std::string readAll() {
        std::string s;
        std::ostringstream os;
        os << rdbuf();
        return os.str();
    }

private:
    ziStream(void);
    ziStream(const ziStream&);
    ziStream& operator=(const ziStream&);

    ziStreamBuffer* _streambuf;
};

class zoStream : public std::ostream {
public:
    zoStream(const std::string& fileName);
    ~zoStream();

    bool isOK(void);

private:
    zoStream(void);
    zoStream(const zoStream&);
    zoStream& operator=(const zoStream&);

    zoStreamBuffer* _streambuf;
};
