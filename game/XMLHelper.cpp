// Description:
//   XML helper to help load xml docs.
//
// Copyright (C) 2001 Frank Becker
//
#include "XMLHelper.hpp"

#include <memory>

#include "Trace.hpp"
#include "ResourceManager.hpp"

using namespace std;

TiXmlDocument* XMLHelper::load(const string& filename) {
    XTRACE();

    int rSize = ResourceManagerS::instance()->getResourceSize(filename);
    std::unique_ptr<ziStream> infileP(ResourceManagerS::instance()->getInputStream(filename));
    if (!infileP) {
        LOG_ERROR << "Unable to open: [" << filename << "]" << endl;
        return 0;
    }
    ziStream& infile = *infileP;

    char* buf = new char[rSize + 1];
    infile.read(buf, rSize);
    streamsize c = infile.gcount();

    //    LOG_INFO << "LevePack : " << filename
    //             << " (" << c << "/" << rSize << ")" << endl;
    //    LOG_INFO << buf << endl;
    buf[c] = '\0';

    TiXmlDocument* doc = new TiXmlDocument();
    doc->Parse(buf);
    delete[] buf;
    //    doc->Print(stdout);

    if (doc->Error()) {
        LOG_ERROR << "Failed to parse xml file: [" << filename << "]" << endl;
        LOG_ERROR << "--> XML: " << doc->ErrorDesc() << endl;
        delete doc;
        return 0;
    }

    return doc;
}
