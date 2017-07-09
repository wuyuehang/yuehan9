bin_PROGRAMS =

AM_CFLAGS = $(GCC_CFLAGS)
AM_CXXFLAGS = $(GXX_CFLAGS)
LDADD = @X11_LIBS@ @GLU_LIBS@ @GLEW_LIBS@ @ASSIMP_LIBS@ -lm -lSOIL -lglut

bin_PROGRAMS += triangle
triangle_SOURCES = triangle.cpp
triangle_CXXFLAGS = $(AM_CXXFLAGS)

bin_PROGRAMS += assimp
assimp_SOURCES = assimp.cpp
assimp_CXXFLAGS = $(AM_CXXFLAGS)
