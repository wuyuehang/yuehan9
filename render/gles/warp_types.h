#ifndef _WARP_TYPES_H
#define _WARP_TYPES_H

#include <GLES3/gl3.h>
#include <EGL/egl.h>
#include <EGL/eglext.h>
#include <X11/Xlib.h>
#include <assert.h>
#include "utility.h"

struct warp_runtime {
	/* X11 stuff */
	Window x11_win;
	Display *x11_display;
	XSetWindowAttributes xwin_attrib;
	XEvent event;

	/* egl native */
	EGLNativeDisplayType native_display;
	EGLNativeWindowType native_win;

	/* egl enviroment */
	EGLDisplay egl_display;
	EGLContext egl_context;
	EGLSurface egl_surface;
	EGLConfig egl_config;
	EGLint major;
	EGLint minor;
};

/* bridge X11 with egl and set up context */
void create_warp_runtime(struct warp_runtime *wrt)
{
	/* open X11 and get display handle */
	wrt->x11_display = XOpenDisplay(NULL);
	wrt->xwin_attrib.event_mask = ExposureMask | KeyPressMask;

	wrt->x11_win = XCreateWindow(
			wrt->x11_display,
			DefaultRootWindow(wrt->x11_display),
			0, 0, 500, 500, 0,
			CopyFromParent, InputOutput,
			CopyFromParent, CWEventMask,
			&(wrt->xwin_attrib));

	XMapWindow(wrt->x11_display, wrt->x11_win);

	/* cast x11 window and display handle to egl native type */
	wrt->native_display = (EGLNativeDisplayType) wrt->x11_display;
	wrt->native_win = (EGLNativeWindowType) wrt->x11_win;

	/* create egl display|context|surface */
	wrt->egl_display = eglGetDisplay(wrt->native_display);
	assert(wrt->egl_display != EGL_NO_DISPLAY);

	assert(EGL_TRUE == eglInitialize(wrt->egl_display, &(wrt->major), &(wrt->minor)));

	EGLint numc = 0;
	EGLint attrib[] =
			{
				EGL_RED_SIZE, 8,
				EGL_GREEN_SIZE, 8,
				EGL_BLUE_SIZE,  8,
				EGL_ALPHA_SIZE, 8,
				EGL_DEPTH_SIZE, 24,
				EGL_STENCIL_SIZE, EGL_DONT_CARE,
				EGL_SAMPLE_BUFFERS, 0,
				EGL_RENDERABLE_TYPE, EGL_OPENGL_ES3_BIT_KHR,
				EGL_NONE
			};

	assert(EGL_TRUE == eglChooseConfig(wrt->egl_display, attrib, &(wrt->egl_config), 1, &numc));

	wrt->egl_surface = eglCreateWindowSurface(wrt->egl_display, wrt->egl_config, wrt->native_win, NULL);
	assert(wrt->egl_surface != EGL_NO_SURFACE);

	EGLint ctx_attrib[] =
			{
				EGL_CONTEXT_CLIENT_VERSION, 3,
				EGL_NONE
			};

	wrt->egl_context = eglCreateContext(wrt->egl_display, wrt->egl_config, EGL_NO_CONTEXT, ctx_attrib);
	assert(wrt->egl_context != EGL_NO_CONTEXT);

	eglMakeCurrent(wrt->egl_display, wrt->egl_surface, wrt->egl_surface, wrt->egl_context);
}

void destroy_warp_runtime(struct warp_runtime *wrt)
{
	eglTerminate(wrt->egl_display);
	XUnmapWindow(wrt->x11_display, wrt->x11_win);
	XDestroyWindow(wrt->x11_display, wrt->x11_win);
	XCloseDisplay(wrt->x11_display);
	free(wrt);
}

#define OFFSCREEN_IMAGE	__FILE__".bmp"
#define OFFSCREEN_SURFACE_WIDTH	16
#define OFFSCREEN_SURFACE_HEIGHT	16
// offscreen EGL config
EGLint offscrn_conf_attrib[] =
		{
			EGL_RED_SIZE, 8,
			EGL_GREEN_SIZE, 8,
			EGL_BLUE_SIZE,  8,
			EGL_ALPHA_SIZE, 8,
			EGL_DEPTH_SIZE, 24,
			EGL_STENCIL_SIZE, EGL_DONT_CARE,
			EGL_SAMPLE_BUFFERS, 0,
			EGL_RENDERABLE_TYPE, EGL_OPENGL_ES3_BIT_KHR,
			EGL_SURFACE_TYPE, EGL_PBUFFER_BIT,
			EGL_NONE
		};

// offscreen EGL context
EGLint offscrn_ctx_attrib[] =
		{
			EGL_CONTEXT_CLIENT_VERSION, 3,
			EGL_NONE
		};

// offscreen EGL surface
EGLint offscrn_surface_attrib[] =
		{
			EGL_WIDTH, OFFSCREEN_SURFACE_WIDTH,
			EGL_HEIGHT, OFFSCREEN_SURFACE_WIDTH,
			EGL_NONE
		};

struct offscreen_warp {
	// offscreen EGL blob
	EGLDisplay disp;
	EGLContext ctx;
	EGLSurface sfc;
	EGLContext cfg;
	EGLint major;
	EGLint minor;
	// offscreen gles blob
	GLuint vertex_shader[2];
	GLuint fragment_shader[2];
	GLuint program[2];
};

void create_offscreen_warp(struct offscreen_warp *ow)
{
	ow->disp = eglGetDisplay(EGL_DEFAULT_DISPLAY);
	assert(ow->disp != EGL_NO_DISPLAY);

	assert(EGL_TRUE == eglInitialize(ow->disp, &ow->major, &ow->minor));

	EGLint numc = 0;
	assert(EGL_TRUE == eglChooseConfig(ow->disp, offscrn_conf_attrib, &ow->cfg, 1, &numc));

	ow->ctx = eglCreateContext(ow->disp, ow->cfg, EGL_NO_CONTEXT, offscrn_ctx_attrib);
	assert(ow->ctx != EGL_NO_CONTEXT);

	ow->sfc = eglCreatePbufferSurface(ow->disp, ow->cfg, offscrn_surface_attrib);
	assert(ow->sfc != EGL_NO_SURFACE);

	eglMakeCurrent(ow->disp, ow->sfc, ow->sfc, ow->ctx);

	// preamble operations
	glViewport(0, 0, OFFSCREEN_SURFACE_WIDTH, OFFSCREEN_SURFACE_HEIGHT);
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LESS);

	glClearColor(0.0, 0.0, 0.0, 1.0);
	glClearDepthf(1.0);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

void destroy_offscreen_warp(struct offscreen_warp *ow)
{
	eglMakeCurrent(ow->disp, EGL_NO_SURFACE, EGL_NO_SURFACE, EGL_NO_CONTEXT);
	eglDestroyContext(ow->disp, ow->ctx);
	eglDestroySurface(ow->disp, ow->sfc);
	eglTerminate(ow->disp);
	free(ow);
}

void create_offscreen_warp_shaders(struct offscreen_warp *ow, GLuint *vsidx, char *vspath, GLuint *fsidx, char *fspath)
{
	GLchar message[512];
	GLint status;
	GLchar *source = NULL;

	if (vspath != NULL) {
		*vsidx = glCreateShader(GL_VERTEX_SHADER);
		load_shader_from_file(vspath, &source);
		glShaderSource(*vsidx, 1, (const GLchar * const*)&source, NULL);
		glCompileShader(*vsidx);

		glGetShaderiv(*vsidx, GL_COMPILE_STATUS, &status);
		if (!status) {
			glGetShaderInfoLog(*vsidx, 512, NULL, message);
			printf("%s\n", message);
		}
		free(source);
	}

	if (fspath != NULL) {
		*fsidx = glCreateShader(GL_FRAGMENT_SHADER);
		load_shader_from_file(fspath, &source);
		glShaderSource(*fsidx, 1, (const GLchar * const*)&source, NULL);
		glCompileShader(*fsidx);

		glGetShaderiv(*fsidx, GL_COMPILE_STATUS, &status);
		if (!status) {
			glGetShaderInfoLog(*fsidx, 512, NULL, message);
			printf("%s\n", message);
		}
		free(source);
	}
}

void create_offscreen_warp_program(GLuint *vsidx, GLuint *fsidx, GLuint *prog)
{
	GLint status;
	GLchar message[512];
	assert(*fsidx);
	assert(*vsidx);

	*prog = glCreateProgram();
	glAttachShader(*prog, *vsidx);
	glAttachShader(*prog, *fsidx);
	glLinkProgram(*prog);

	glGetProgramiv(*prog, GL_LINK_STATUS, &status);
	if (!status) {
		glGetProgramInfoLog(*prog, 512, NULL, message);
		printf("%s\n", message);
	}

	glUseProgram(*prog);
}

#endif
