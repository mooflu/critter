#pragma once
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

#include <stdio.h>
#include <string>

#include "Trace.hpp"

class Value {

public:
    enum TypeEnum {
        Unknown = 0,
        String,
        Integer,
        Float,
        Boolean
    };

    Value(std::string s) :
        _valType(String),
        _valString(s),
        _valInt(atoi(s.c_str())),
        _valFloat((float)atof(s.c_str())),
        _valBool(stringToBool(s)) {
        XTRACE();
    }

    Value(int i) :
        _valType(Integer),
        _valString(intToString(i)),
        _valInt(i),
        _valFloat((float)i),
        _valBool(i > 0) {
        XTRACE();
    }

    Value(float f) :
        _valType(Float),
        _valString(floatToString(f)),
        _valInt((int)f),
        _valFloat(f),
        _valBool(f > 0.0f) {
        XTRACE();
    }

    Value(bool b) :
        _valType(Boolean),
        _valString(boolToString(b)),
        _valInt((int)b),
        _valFloat((float)b),
        _valBool(b) {
        XTRACE();
    }

    ~Value() { XTRACE(); }

    TypeEnum getType(void) { return _valType; }

    char* dump(void);

    std::string& getString(void) { return _valString; }

    int getInteger(void) { return _valInt; }

    float getFloat(void) { return _valFloat; }

    bool getBoolean(void) { return _valBool; }

private:
    Value(void);  //dont allow default constr
    Value(const Value&);
    const Value& operator=(const Value&);

    bool stringToBool(std::string& s);
    char* intToString(int i);
    char* floatToString(float f);
    char* boolToString(bool b);

    TypeEnum _valType;

    std::string _valString;
    int _valInt;
    float _valFloat;
    bool _valBool;
};
