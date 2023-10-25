// Description:
//   Tracing for the debug-minded person.
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
#include "Trace.hpp"
#define TRACE

#include <iomanip>
#include <fstream>
using namespace std;

#ifdef HAVE_CONFIG_H
#include <defines.h>  //PACKAGE and VERSION
#endif

int Trace::indent_ = 0;

#ifdef WIN32
static inline ofstream& tcout(void) {
    static ofstream* _tcout = 0;
    if (_tcout == 0) {
        _tcout = new ofstream("trace.txt");
    }
    return *_tcout;
}
#else
static inline ostream& tcout(void) {
    return cout;
}
#endif

Trace::Trace(const char* class_name, const char* method, const char* params) {
    tcout() << setw(indent_) << "" << class_name << "::" << method << "(" << params << ") {" << endl;
    indent_++;
}

Trace::Trace(const char* function, const char* params) {
    tcout() << setw(indent_) << ""
            << "::" << function << "(" << params << ") {" << endl;
    indent_++;
}

Trace::Trace(const char* str) {
    tcout() << setw(indent_) << "" << str << " {" << endl;
    indent_++;
}

Trace::~Trace() {
    indent_--;
    tcout() << setw(indent_) << ""
            << "}" << endl;
}

#ifdef TRACE
#ifdef WIN32
#include "../utilsfs/GetDataPath.hpp"
ofstream mcout((getWritableDataPath(PACKAGE) + "/log.txt").c_str());
//#define mcout cout
#else
#define mcout cout
#endif

void Trace::SetStreamBuffer(streambuf* newBuffer) {
#ifndef WIN32
    /*streambuf *old =*/mcout.rdbuf(newBuffer);
    //delete old; //This breaks on iPhone with a 'pointer being freed was not allocated'
#endif
}

ostream& Trace::Log(int severity) {
    const char* type;

    switch (severity) {
        case Trace::eDEBUG:
            type = "DEBUG";
            break;
        case Trace::eINFO:
            type = "INFO";
            break;
        case Trace::eWARNING:
            type = "WARNING";
            break;
        case Trace::eERROR:
            type = "*ERROR*";
            break;
        case Trace::eFATAL:
            type = "FATAL";
            break;

        case Trace::eVOID:
            return mcout;

        default:
            type = "TRACE";
            break;
    }

    mcout << setw(indent_) << "" << type << ": ";
#else
ostream& Trace::Log(int) {
#endif
    return mcout;
}

#ifdef DEBUG_TRACE
class MyClass {
public:
    MyClass(void);
    ~MyClass();
};

MyClass::MyClass(void) {
    MTRACE("MyClass", "MyClass", "void");

    if (1) {
        STRACE("if(1)");
        LOG_INFO << "We always do this!" << endl;
    }
}

MyClass::~MyClass() {
    MTRACE("MyClass", "~MyClass", "");
}

void myFunc(void) {
    FTRACE("myFunc", "void");
    int severity = Trace::FATAL;

    LOG(severity) << "hallo" << endl;
    LOG_INFO << "info" << endl;
    LOG_WARNING << "warning" << endl;
    LOG_ERROR << "error" << endl;
}

int main(int, char*[]) {
    FTRACE("main", "int, char *[]");
    MyClass c;

    myFunc();

    return 0;
}
#endif

#ifdef IDEAS
{
    Trace trace("CLASS::METHOD(PARAMS)");
    Trace trace("CLASS", "METHOD", "PARAMS");

    //timestamping
    Trace::EnableTimestamp();
    Trace::DisableTimestamp();

    //logging with severity
    Trace::Debug(...)
	Trace::Info(...)
	Trace::Warning(...)
	Trace::Error(...)
	Trace::Fatal(...)
	Trace::Log( severity, ...);

    //enables debug for given severities
    Trace::EnableSeverity(DEBUG | INFO | ALL_SEVERITIES);
    Trace::DisableSeverity(DEBUG);

    //sends output to file
    Trace::AddOutput(file);
    Trace::DelOutput(file);

    //config options
    Trace::ProcessCommandline(argc, argv);

#ifdef FANCY_TRACE
    //config options
    Trace::ProcessConfigFile(file);

    //listens and sends output to this ip:port
    Trace::AddOutput(ip, port);
    Trace::DelOutput(ip, port);

    //listens and accepts commands from port
    Trace::AddCommandPort(ip, port);
    Trace::DelCommandPort(ip, port);

    //select which classes and methods to trace
    Trace::Disable("CLASS" | NULL, "METHOD" | NULL, "PARAMS" | NULL);
    Trace::Enable("CLASS" | NULL, "METHOD" | NULL, "PARAMS" | NULL);
#endif  //FANCE_TRACE
}
#endif
