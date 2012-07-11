#pragma once
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
#include <iostream>

//#define TRACE
#ifdef TRACE

#define XTRACE() Trace __TRACE_pretty_Function__((__PRETTY_FUNCTION__))

//Method trace
#define MTRACE(_class_,_method_,_params_) \
  Trace __TRACE_MeThOd__((_class_),(_method_),(_params_))

//Function trace
#define FTRACE(_func_,_params_) \
  Trace __TRACE_FuNcTiOn__((_func_),(_params_))

//String trace
#define STRACE(_string_) \
  Trace __TRACE_StRiNg__((_string_))

#define LOG(_severity_) Trace::Log(_severity_)
#define LOG_DEBUG       Trace::Log(Trace::eDEBUG)
#define LOG_INFO        Trace::Log(Trace::eINFO)
#define LOG_WARNING     Trace::Log(Trace::eWARNING)
#define LOG_ERROR       Trace::Log(Trace::eERROR)
#define LOG_FATAL       Trace::Log(Trace::eFATAL)
#define LOG_VOID        Trace::Log(Trace::eVOID)

#define LOG_FILELINE    (Trace::Log(Trace::eWARNING) << __FILE__ << ":" << __LINE__ << " ")

#else

#define XTRACE()
#define MTRACE(_class_,_method_,_params_)
#define FTRACE(_funct_,_params_)
#define STRACE(_string_)

#define LOG(_severity) if(1);else Trace::Log(_severity_)
#define LOG_DEBUG      if(1);else Trace::Log(Trace::eDEBUG)

#define LOG_INFO       Trace::Log(Trace::eINFO)
#define LOG_WARNING    Trace::Log(Trace::eWARNING)
#define LOG_ERROR      Trace::Log(Trace::eERROR)
#define LOG_FATAL      Trace::Log(Trace::eFATAL)
#define LOG_VOID       Trace::Log(Trace::eVOID)

#define LOG_FILELINE    (Trace::Log(Trace::eWARNING) << __FILE__ << ":" << __LINE__ << " ")

#if 0
#define LOG_INFO       if(1);else Trace::Log(Trace::eINFO)
#define LOG_WARNING    if(1);else Trace::Log(Trace::eWARNING)
#define LOG_ERROR      if(1);else Trace::Log(Trace::eERROR)
#define LOG_FATAL      if(1);else Trace::Log(Trace::eFATAL)
#endif

#endif

class Trace
{
public:
    enum
    {
	eDEBUG,
	eINFO,
	eWARNING,
	eERROR,
	eFATAL,
	eVOID
    };

    Trace( const char *class_name, const char *method, const char *params);
    Trace( const char *function, const char *params);
    Trace( const char *str);
    ~Trace();

    static void SetStreamBuffer( std::streambuf *newBuffer);

    static std::ostream &Log( int severity);

    static int indent_;
};
