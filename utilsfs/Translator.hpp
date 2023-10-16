#pragma once
// Description:
//   Translator for I18N.
//
// Copyright (C) 2011 Frank Becker
//

#include "Singleton.hpp"
#include "HashString.hpp"

#include <string>

class Translator {
    friend class Singleton<Translator>;

public:
    void loadDictionary(const std::string& code);
    bool translate(const std::string& lookup, const std::string& context, std::string& result);

private:
    Translator(void){};
    virtual ~Translator(){};

    Translator(const Translator&);
    Translator& operator=(const Translator&);

    hash_map<const std::string, std::string, hash<const std::string>, std::equal_to<const std::string>> _dict;

    std::string _code;
};

typedef Singleton<Translator> TranslatorS;

static inline std::string trans(const std::string& lookup, const std::string& context = std::string("")) {
    std::string result;
    TranslatorS::instance()->translate(lookup, context, result);
    return result;
}
