bin_PROGRAMS =

AM_CFLAGS = $(GCC_CFLAGS)
AM_CXXFLAGS = $(GXX_CFLAGS)
LDADD = @X11_LIBS@ @GLFW3_LIBS@ @GLEW_LIBS@ -lm -lSOIL

bin_PROGRAMS += texture2d
texture2d_SOURCES = texture2d.cpp
texture2d_CXXFLAGS = $(AM_CXXFLAGS)

bin_PROGRAMS += glfw_gs_point2line
glfw_gs_point2line_SOURCES = glfw_gs_point2line.c
glfw_gs_point2line_CFLAGS = $(AM_CFLAGS)

bin_PROGRAMS += glfw_gs_point2triangle
glfw_gs_point2triangle_SOURCES = glfw_gs_point2triangle.c
glfw_gs_point2triangle_CFLAGS = $(AM_CFLAGS)

bin_PROGRAMS += glfw_vs_feedback_point
glfw_vs_feedback_point_SOURCES = glfw_vs_feedback_point.cpp
glfw_vs_feedback_point_CXXFLAGS = $(AM_CXXFLAGS)

bin_PROGRAMS += dual_vao
dual_vao_SOURCES = dual_vao.cpp
dual_vao_CXXFLAGS = $(AM_CXXFLAGS)

bin_PROGRAMS += phong_model
phong_model_SOURCES = phong_model.cpp
phong_model_CXXFLAGS = $(AM_CXXFLAGS)

bin_PROGRAMS += uniform
uniform_SOURCES = uniform.cpp
uniform_CXXFLAGS = $(AM_CXXFLAGS)

bin_PROGRAMS += renderbuffer
renderbuffer_SOURCES = renderbuffer.cpp
renderbuffer_CXXFLAGS = $(AM_CXXFLAGS)

bin_PROGRAMS += mrt
mrt_SOURCES = mrt.cpp
mrt_CXXFLAGS = $(AM_CXXFLAGS)

bin_PROGRAMS += query
query_SOURCES = query.cpp
query_CXXFLAGS = $(AM_CXXFLAGS)

bin_PROGRAMS += rendertexture
rendertexture_SOURCES = rendertexture.c
rendertexture_CFLAGS = $(AM_CFLAGS)

bin_PROGRAMS += attrib
attrib_SOURCES = attrib.c
attrib_CFLAGS = $(AM_CFLAGS)

bin_PROGRAMS += instance
instance_SOURCES = instance.cpp
instance_CXXFLAGS = $(AM_CXXFLAGS)
