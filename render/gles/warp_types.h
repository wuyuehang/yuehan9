#include <GLES3/gl3.h>
#include <EGL/egl.h>
#include <EGL/eglext.h>
#include <X11/Xlib.h>

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
				EGL_ALPHA_SIZE, EGL_DONT_CARE,
				EGL_DEPTH_SIZE, EGL_DONT_CARE,
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
