bin_PROGRAMS =
AM_CFLAGS = $(GCC_CFLAGS)
AM_CXXFLAGS = $(GXX_CFLAGS)
LDADD = @X11_LIBS@ @GLES_LIBS@ @EGL_LIBS@

bin_PROGRAMS += egl_clear
egl_clear_SOURCES = egl_clear.c
egl_clear_CFLAGS = $(AM_CFLAGS)

bin_PROGRAMS += egl_template
egl_template_SOURCES = egl_template.c
egl_templater_CFLAGS = $(AM_CFLAGS)

bin_PROGRAMS += es3_draw_tri
es3_draw_tri_SOURCES = es3_draw_tri.c
es3_draw_tri_CFLAGS = $(AM_CFLAGS)

bin_PROGRAMS += es3_draw_rect
es3_draw_rect_SOURCES = es3_draw_rect.c
es3_draw_rect_CFLAGS = $(AM_CFLAGS)

bin_PROGRAMS += es3_cull
es3_cull_SOURCES = es3_cull.c
es3_cull_CFLAGS = $(AM_CFLAGS)

bin_PROGRAMS += es3_dual_vao
es3_dual_vao_SOURCES = es3_dual_vao.cpp
es3_dual_vao_CXXFLAGS = $(AM_CXXFLAGS)
