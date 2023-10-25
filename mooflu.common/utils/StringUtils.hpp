#pragma once
// Description:
//   String helpers.
//
// Copyright (C) 2012 Frank Becker
//
#include <algorithm>
#include <string>
#include <sstream>
#include <vector>

class StringUtils {
public:
    StringUtils(const std::string& str) :
        _str(str) {}

    StringUtils& toLower(void) {
        std::transform(_str.begin(), _str.end(), _str.begin(), (int (*)(int))std::tolower);
        return *this;
    }

    StringUtils& toUpper(void) {
        std::transform(_str.begin(), _str.end(), _str.begin(), (int (*)(int))std::toupper);
        return *this;
    }

    StringUtils& replaceAll(const std::string& f, const std::string& r) {
        const std::string& s = _str;

        if (s.empty() || f.empty() || f == r || f.size() > s.size() || s.find(f) == std::string::npos) {
            return *this;
        }
        std::ostringstream build_it;
        typedef std::string::const_iterator iter;
        iter i(s.begin());
        const iter::difference_type f_size(distance(f.begin(), f.end()));
        for (iter pos; (pos = std::search(i, s.end(), f.begin(), f.end())) != s.end();) {
            std::copy(i, pos, std::ostreambuf_iterator<char>(build_it));
            std::copy(r.begin(), r.end(), std::ostreambuf_iterator<char>(build_it));
            std::advance(pos, f_size);
            i = pos;
        }
        std::copy(i, s.end(), std::ostreambuf_iterator<char>(build_it));
        _str = build_it.str();

        return *this;
    }

    std::vector<std::string> csvToVector(char delimiter = ',', char enclosure = '"') const {
        std::vector<std::string> result;
        result.push_back("");

        int idx = 0;
        bool quoted = false;
        for (unsigned int i = 0; i < _str.length(); i++) {
            char ch = _str[i];
            if (ch == enclosure) {
                quoted = !quoted;
            }

            if (ch == delimiter && !quoted) {
                result.push_back("");
                idx++;
            } else {
                result[idx] += ch;
            }
        }

        std::string escapedEnclosure = enclosure + std::string("") + enclosure;
        for (unsigned int k = 0; k < result.size(); k++) {
            if (result[k] != "" && result[k][0] == enclosure) {
                result[k] = result[k].substr(1, result[k].length() - 2);
                result[k] = StringUtils(result[k]).replaceAll(escapedEnclosure, std::string("") + enclosure);
            }
        }

        return result;
    }

    bool endsWith(const std::string& ending) const {
        if (_str.length() >= ending.length()) {
            return (_str.compare(_str.length() - ending.length(), ending.length(), ending) == 0);
        }

        return false;
    }

    std::string& str() { return _str; }

    operator std::string() const { return _str; }

protected:
    std::string _str;
};
