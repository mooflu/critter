// Description:
//   XXTea implementation.
//
// See http://en.wikipedia.org/wiki/XXTEA
//
// Except for encode/decode cores:
// Copyright (C) 2009 Frank Becker

#include <Tea.hpp>

#define MX ((z >> 5 ^ y << 2) + (y >> 3 ^ z << 4)) ^ ((sum ^ y) + (k[(p & 3) ^ e] ^ z));

uint32_t Tea::k[4] = {
    0x7ef6a7c3,
    0xe8452af1,
    0x089bce5d,
    0xcae63b12,
};

void Tea::msgToData(const std::string& msg, std::vector<uint32_t>& v) {
    size_t n = (msg.size() + 3) / 4;
    size_t len = msg.size();
    size_t pos = 0;
    for (size_t i = 0; i < n; i++) {
        uint32_t result = (unsigned char)msg[pos++];
        if (pos < len) {
            result += (unsigned char)msg[pos++] << (1 * 8);
        }
        if (pos < len) {
            result += (unsigned char)msg[pos++] << (2 * 8);
        }
        if (pos < len) {
            result += (unsigned char)msg[pos++] << (3 * 8);
        }

        v.push_back(result);
    }
}

std::string Tea::dataToMsg(const std::vector<uint32_t>& v) {
    std::string result(v.size() * 4, 0);

    unsigned int pos = 0;
    for (unsigned int i = 0; i < v.size(); i++) {
        uint32_t value = v[i];
        result[pos++] = value & 0xff;
        result[pos++] = (value >> (1 * 8)) & 0xff;
        result[pos++] = (value >> (2 * 8)) & 0xff;
        result[pos++] = (value >> (3 * 8)) & 0xff;
    }

    return result;
}

std::string Tea::encode(const std::string& msg) {
    std::vector<uint32_t> v;
    msgToData(msg, v);
    unsigned int n = (unsigned int)v.size();

    unsigned p, e;
    unsigned int rounds = 6 + 52 / n;
    uint32_t sum = 0;
    uint32_t y;
    uint32_t z = v[n - 1];

    do {
        sum += DELTA;
        e = (sum >> 2) & 3;
        for (p = 0; p < n - 1; p++) {
            y = v[p + 1];
            v[p] += MX;
            z = v[p];
        }
        y = v[0];
        v[n - 1] += MX;
        z = v[n - 1];

    } while (--rounds);

    return dataToMsg(v);
}

std::string Tea::decode(const std::string& msg) {
    std::vector<uint32_t> v;
    msgToData(msg, v);
    unsigned int n = (unsigned int)v.size();

    unsigned p, e;
    unsigned int rounds = 6 + 52 / n;
    uint32_t sum = rounds * DELTA;
    uint32_t y = v[0];
    uint32_t z;

    do {
        e = (sum >> 2) & 3;
        for (p = n - 1; p > 0; p--) {
            z = v[p - 1];
            v[p] -= MX;
            y = v[p];
        }
        z = v[n - 1];
        v[0] -= MX;
        y = v[0];

    } while ((sum -= DELTA) != 0);

    return dataToMsg(v);
}

#if 0
#include <iostream>
int main( int argc, char *argv[])
{
    std::string msg = argv[1];

    std::cout << "Encoding: [" << msg << "]\n";
    std::string e = Tea::encode(msg);

    std::cout << "Decoding: [" << e << "]\n";
    std::string d = Tea::decode(e);

    std::cout << "Result: [" << d << "]\n";

    return 0;
}
#endif
