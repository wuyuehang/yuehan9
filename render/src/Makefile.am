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

bin_PROGRAMS += perspective_correct
perspective_correct_SOURCES = perspective_correct.cpp
perspective_correct_CXXFLAGS = $(AM_CXXFLAGS)

bin_PROGRAMS += draw_xfb
draw_xfb_SOURCES = draw_xfb.c
draw_xfb_CFLAGS = $(AM_CFLAGS)

bin_PROGRAMS += separate_shader_obj
separate_shader_obj_SOURCES = separate_shader_obj.c
separate_shader_obj_CFLAGS = $(AM_CFLAGS)

bin_PROGRAMS += fragcord
fragcord_SOURCES = fragcord.c
fragcord_CFLAGS = $(AM_CFLAGS)

bin_PROGRAMS += stencil
stencil_SOURCES = stencil.c
stencil_CFLAGS = $(AM_CFLAGS)

bin_PROGRAMS += indirect
indirect_SOURCES = indirect.c
indirect_CFLAGS = $(AM_CFLAGS)

bin_PROGRAMS += skybox
skybox_SOURCES = skybox.cpp
skybox_CXXFLAGS = $(AM_CXXFLAGS)

bin_PROGRAMS += perspective_tune
perspective_tune_SOURCES = perspective_tune.cpp
perspective_tune_CXXFLAGS = $(AM_CXXFLAGS)
