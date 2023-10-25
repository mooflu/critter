// Description:
//   Translator for I18N.
//
// Copyright (C) 2011 Frank Becker
//
#include "Translator.hpp"

#include "Trace.hpp"
#include "StringUtils.hpp"
#include "ResourceManager.hpp"

#include <memory>
using namespace std;

void Translator::loadDictionary(const string& code) {
    if (_code == code) {
        return;
    }

    _code = code;
    _dict.clear();

    string filePath = "translations/" + _code + ".csv";

    if (ResourceManagerS::instance()->hasResource(filePath)) {
        LOG_INFO << "Loading translations for " << _code << "\n";
        shared_ptr<ziStream> dictFilePtr(ResourceManagerS::instance()->getInputStream(filePath));
        ziStream& dictFile = *dictFilePtr;

        string line;
        while (!getline(dictFile, line).eof()) {
            //remove any trailing LF/CR
            string::size_type pos = line.find_last_not_of("\r\n");
            if (pos != string::npos) {
                line = line.substr(0, pos + 1);
            }

            vector<string> params = StringUtils(line).csvToVector();
            if (params.size() == 2) {
                _dict[params[0]] = params[1];
            }
        }
        LOG_INFO << "Translations dictionary contains " << _dict.size() << " entries.\n";
    } else {
        LOG_ERROR << "Failed to find translation file for " << _code << "\n";
    }
}

bool Translator::translate(const string& lookup, const string& context, string& result) {
    hash_map<const string, string, hash<const string>>::const_iterator ci;
    ci = _dict.find(lookup);
    if (ci == _dict.end()) {
        if (true)  //gGameState->
        {
            result = "<error>" + lookup + "</error>";
        } else {
            result = lookup;
        }

        return false;
    }

    result = ci->second;
    return true;
}
