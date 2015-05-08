noinst_LIBRARIES = libutils.a

libutilsincludedir = $(includedir)/SDL 

libutils_a_SOURCES = \
	Config.cpp \
	GetDataPath.cpp \
	Endian.cpp \
	FPS.cpp \
	RectanglePacker.cpp \
	Random.cpp \
        RateLimiter.cpp \
	R250.cpp \
	ResourceManager.cpp \
	Trace.cpp \
	Value.cpp \
	WalkDirectory.cpp \
	zStream.cpp \
	zStreamBufferImplLZMA.cpp \
	zStreamBufferImplZLib.cpp

DEFS += -DDATA_DIR=\"$(pkgdatadir)/\"

if USE_LZMA
libutils_a_SOURCES += \
	lzma/SRC/7zip/Compress/LZMA/LZMADecoder.cpp \
	lzma/SRC/7zip/Compress/LZMA/LZMAEncoder.cpp \
	lzma/SRC/7zip/Common/InBuffer.cpp \
	lzma/SRC/7zip/Common/OutBuffer.cpp \
	lzma/SRC/7zip/Compress/LZ/LZInWindow.cpp \
	lzma/SRC/7zip/Compress/LZ/LZOutWindow.cpp \
	lzma/SRC/7zip/Compress/RangeCoder/RangeCoderBit.cpp \
	lzma/SRC/Common/Alloc.cpp \
	lzma/SRC/Common/CRC.cpp
INCLUDES = -DUSE_LZMA -Ilzma/SRC/7zip/Compress/LZMA -D_NO_EXCEPTIONS
endif

depend:
	makedepend -I/usr/include/g++-3 $(DEFS) $(INCLUDES) $(AM_CPPFLAGS) $(CPPFLAGS) $(AM_CXXFLAGS) $(CXXFLAGS) $(libutils_a_SOURCES)

MAINTAINERCLEANFILES = Makefile.bak Makefile.in
