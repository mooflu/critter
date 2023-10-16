#pragma once
// Description:
//   Helper to tokenize a string.
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

#include <string>
#include <vector>

#include <iostream>
#include <algorithm>
#include <iterator>

class Tokenizer {
public:
    Tokenizer(std::string line, const char* whitespace = " \t\n\r") :
        _line(line),
        _whitespace(whitespace),
        _pos(0),
        _tokenCount(0),
        _withQuotes(true) {}

    Tokenizer(std::string line, bool withQuotes, const char* whitespace = " \t\n\r") :
        _line(line),
        _whitespace(whitespace),
        _pos(0),
        _tokenCount(0),
        _withQuotes(withQuotes) {}

    std::string operator()(void) { return next(); }

    void setWhitespace(const std::string& whitespace) { _whitespace = whitespace; }

    std::string next(void) {
        std::string retVal = "";
        std::string::size_type start = _line.find_first_not_of(_whitespace, _pos);
        std::string::size_type adj = 0;
        if (start != std::string::npos) {
            if (_withQuotes && (_line[start] == '"')) {
                start++;
                _pos = _line.find_first_of("\"", start);
                _pos++;
                adj = 1;
            } else {
                _pos = _line.find_first_of(_whitespace, start);
            }

            if (_pos == std::string::npos) {
                retVal = _line.substr(start);
            } else {
                retVal = _line.substr(start, _pos - start - adj);
            }
            _tokenCount++;
        }

        return retVal;
    }

    int tokensReturned(void) { return _tokenCount; }

    std::string::size_type getPos(void) { return _pos; }

    void setPos(std::string::size_type pos) { _pos = pos; }

private:
    std::string _line;
    std::string _whitespace;
    std::string::size_type _pos;

    int _tokenCount;
    bool _withQuotes;
};
