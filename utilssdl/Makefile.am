noinst_LIBRARIES = libutilssdl.a

libutilssdlincludedir = $(includedir)/SDL

libutilssdl_a_SOURCES = \
	Audio.cpp \
	SampleManager.cpp \
	PNG.cpp \
	zrwops.cpp

INCLUDES = -I../utils

depend:
	makedepend -I/usr/include/g++-3 $(DEFS) $(INCLUDES) $(AM_CPPFLAGS) $(CPPFLAGS) $(AM_CXXFLAGS) $(CXXFLAGS) $(libutilssdl_a_SOURCES)

MAINTAINERCLEANFILES = Makefile.bak Makefile.in
