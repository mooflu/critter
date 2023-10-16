#pragma once
// Description:
//   XXTea implementation.
//
// Copyright (C) 2009 Frank Becker

#include <string>
#include <vector>

//The input msg must be >4 chars.
//The block size is variable but should be at least 2 32-bit words (>4 chars).
#ifndef uint32_t
typedef unsigned int uint32_t;
#endif

class Tea {
private:
    static const uint32_t DELTA = 0x9e3779b9;
    static void msgToData(const std::string& msg, std::vector<uint32_t>& v);
    static std::string dataToMsg(const std::vector<uint32_t>& v);

public:
    static uint32_t k[4];
    static std::string encode(const std::string& msg);
    static std::string decode(const std::string& msg);
};
