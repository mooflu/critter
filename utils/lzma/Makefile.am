noinst_LIBRARIES = liblzma.a

AM_CPPFLAGS = -I$(srcdir)/SRC/7zip/Compress/LZMA -D_NO_EXCEPTIONS

liblzmaincludedir = $(includedir)/SDL

liblzma_a_SOURCES = \
	SRC/7zip/Compress/LZMA/LZMADecoder.cpp \
	SRC/7zip/Compress/LZMA/LZMAEncoder.cpp \
	SRC/7zip/Common/InBuffer.cpp \
	SRC/7zip/Common/OutBuffer.cpp \
	SRC/7zip/Compress/LZ/LZInWindow.cpp \
	SRC/7zip/Compress/LZ/LZOutWindow.cpp \
	SRC/7zip/Compress/RangeCoder/RangeCoderBit.cpp \
	SRC/Common/Alloc.cpp \
	SRC/Common/CRC.cpp

depend:
	makedepend -I/usr/include/g++-3 $(DEFS) $(INCLUDES) $(AM_CPPFLAGS) $(CPPFLAGS) $(AM_CXXFLAGS) $(CXXFLAGS) $(liblzma_a_SOURCES)

MAINTAINERCLEANFILES = Makefile.bak Makefile.in
