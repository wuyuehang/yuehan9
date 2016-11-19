bin_PROGRAMS =
AM_CFLAGS = $(GCC_CFLAGS)
LDADD = @X11_LIBS@ @GLU_LIBS@ @GLFW3_LIBS@ @GLEW_LIBS@ -lm

bin_PROGRAMS += basic-win
basic_win_SOURCES = basic-win.c
basic_win_CFLAGS = $(AM_CFLAGS)

bin_PROGRAMS += simple-ogl
simple_ogl_SOURCES = simple-ogl.c
simple_ogl_CFLAGS = $(AM_CFLAGS)

bin_PROGRAMS += simple-connect
simple_connect_SOURCES = simple-connect.c
simple_connect_CFLAGS = $(AM_CFLAGS)

bin_PROGRAMS += draw-line
draw_line_SOURCES = draw-line.c
draw_line_CFLAGS = $(AM_CFLAGS)

bin_PROGRAMS += draw-color
draw_color_SOURCES = draw-color.c
draw_color_CFLAGS = $(AM_CFLAGS)

bin_PROGRAMS += draw-font
draw_font_SOURCES = draw-font.c
draw_font_CFLAGS = $(AM_CFLAGS)

# skip glut dependency build binary
#bin_PROGRAMS += gl-triangle
#gl_triangle_SOURCES = gl-triangle.c
#gl_triangle_CFLAGS = $(AM_CFLAGS)

#bin_PROGRAMS += gl-vertex
#gl_vertex_SOURCES = gl-vertex.c
#gl_vertex_CFLAGS = $(AM_CFLAGS)

#bin_PROGRAMS += gl-line
#gl_line_SOURCES = gl-line.c
#gl_line_CFLAGS = $(AM_CFLAGS)

#bin_PROGRAMS += minimal
#minimal_SOURCES = minimal.c
#minimal_CFLAGS = $(AM_CFLAGS)

#bin_PROGRAMS += pyramid
#pyramid_SOURCES = pyramid.c
#pyramid_CFLAGS = $(AM_CFLAGS)

#bin_PROGRAMS += infos
#infos_SOURCES = infos.c
#infos_CFLAGS = $(AM_CFLAGS)

bin_PROGRAMS += glfw_demo
glfw_demo_SOURCES = glfw_demo.c
glfw_CFLAGS = $(AM_CFLAGS)

bin_PROGRAMS += glfw_draw_tri
glfw_draw_tri_SOURCES = glfw_draw_tri.c
glfw_draw_tri_CFLAGS = $(AM_CFLAGS)
