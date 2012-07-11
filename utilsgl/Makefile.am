noinst_LIBRARIES = libutilsgl.a

libutilsglincludedir = $(includedir)/SDL

libutilsgl_a_SOURCES = \
        Camera.cpp \
	GLBitmapCollection.cpp \
	GLBitmapFont.cpp \
	GLExtension.cpp \
	GLMultiTexture.cpp \
	GLVertexArrayEXT.cpp \
	GLTexture.cpp \
	GLTextureCubeMap.cpp \
	GLVertexBufferObject.cpp \
	Model.cpp \
        ModelManager.cpp \
	TextureManager.cpp \
	FontManager.cpp \
	BitmapManager.cpp \
	GLee.c

INCLUDES = -I../utils -I../utilssdl

depend:
	makedepend -I/usr/include/g++-3 $(DEFS) $(INCLUDES) $(AM_CPPFLAGS) $(CPPFLAGS) $(AM_CXXFLAGS) $(CXXFLAGS) $(libutilsgl_a_SOURCES)

MAINTAINERCLEANFILES = Makefile.bak Makefile.in
