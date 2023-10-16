// Description:
//   Helper to determine endian-ness.
//
// Copyright (C) 2001 Frank Becker
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
#include "Endian.hpp"

#include <stdlib.h>
#include "Trace.hpp"

enum EndianEnum {
    eNeedInit = 0,
    eLittleEndian = 0x34333231,
    eBigEndian = 0x31323334,
    eUnknown,
    eLAST
};

static EndianEnum _endianess = eNeedInit;

bool isLittleEndian(void) {
    XTRACE();
    switch (_endianess) {
        case eNeedInit: {
            char c[4] = {'1', '2', '3', '4'};
            _endianess = (EndianEnum) * ((int*)c);
        }
            return _endianess == eLittleEndian;
            break;
        case eLittleEndian:
            return true;
        case eBigEndian:
            return false;
        default:
            LOG_ERROR << "Unsupported endianess (PDP?). You are weird!" << std::endl;
            exit(-1);
    }

    //shouldn't get here
    return false;
}
