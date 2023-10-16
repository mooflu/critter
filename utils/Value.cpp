// Description:
//   String value storage and conversion helpers.
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
#include "Value.hpp"

#ifndef snprintf
#define snprintf(a, b, c, d) sprintf((a), (c), (d))
#endif
bool Value::stringToBool(std::string& s) {
    //    XTRACE();
    if ((s == "true") || (s == "True") || (s == "TRUE") || (s == "on") || (s == "On") || (s == "ON") || (s == "1")) {
        return true;
    }

    return false;
}

char* Value::intToString(int i) {
    //    XTRACE();
    static char buf[256];
    snprintf(buf, sizeof(buf), "%d", i);
    return buf;
}

char* Value::floatToString(float f) {
    //    XTRACE();
    static char buf[256];
    snprintf(buf, sizeof(buf), "%f", f);
    return buf;
}

char* Value::boolToString(bool b) {
    //    XTRACE();
    static char buf[256];
    snprintf(buf, sizeof(buf), "%d", b);
    return buf;
}

char* Value::dump(void) {
    XTRACE();
    static char buf[256];

    switch (_valType) {
        case String:
            snprintf(buf, sizeof(buf), "string [%s]", _valString.c_str());
            break;
        case Integer:
            snprintf(buf, sizeof(buf), "int [%d]", _valInt);
            break;
        case Float:
            snprintf(buf, sizeof(buf), "float [%f]", _valFloat);
            break;
        case Boolean:
            snprintf(buf, sizeof(buf), "bool [%d]", _valBool);
            break;
        default:
            break;
    }

    return (buf);
}
