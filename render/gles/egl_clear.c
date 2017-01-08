#include <assert.h>
#include <GLES3/gl3.h>
#include <EGL/egl.h>
#include <EGL/eglext.h>
#include <X11/Xlib.h>

EGLNativeDisplayType native_display;
EGLNativeWindowType native_win;

EGLDisplay egl_display;
EGLContext egl_context;
EGLSurface egl_surface;
EGLConfig egl_config;

EGLint major, minor;

EGLint ctx_attrib[] =
		{
			EGL_CONTEXT_CLIENT_VERSION, 3,
			EGL_NONE
		};

Display *x11_display = NULL;

int main(int argc, char **argv)
{
	Window root;
	Window win;
	XEvent event;
	XSetWindowAttributes xwin_attrib;

	/* open X11 and get display handle */
	x11_display = XOpenDisplay(NULL);
	root = DefaultRootWindow(x11_display);

	xwin_attrib.event_mask = ExposureMask | KeyPressMask;

	win = XCreateWindow(
			x11_display, root,
			0, 0, 500, 500, 0,
			CopyFromParent, InputOutput,
			CopyFromParent, CWEventMask,
			&xwin_attrib);

	XMapWindow(x11_display, win);

	/* cast x11 window and display handle to egl native type */
	native_display = (EGLNativeDisplayType) x11_display;
	native_win = (EGLNativeWindowType) win;

	egl_display = eglGetDisplay(native_display);
	assert(egl_display != EGL_NO_DISPLAY);

	assert(EGL_TRUE == eglInitialize(egl_display, &major, &minor));

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

	assert(EGL_TRUE == eglChooseConfig(egl_display, attrib, &egl_config, 1, &numc));

	egl_surface = eglCreateWindowSurface(egl_display, egl_config, native_win, NULL);
	assert(egl_surface != EGL_NO_SURFACE);

	egl_context = eglCreateContext(egl_display, egl_config, EGL_NO_CONTEXT, ctx_attrib);
	assert(egl_context != EGL_NO_CONTEXT);

	eglMakeCurrent(egl_display, egl_surface, egl_surface, egl_context);

	glViewport(0, 0, 500, 500);

	while (1) {

		XNextEvent(x11_display, &event);

		switch(event.type) {
		case Expose:
			glClearColor(1.0f, 0.0f, 0.0f, 0.0f);
			glClear(GL_COLOR_BUFFER_BIT);
			eglSwapBuffers(egl_display, egl_surface);
			break;
		case ButtonPress:
		case KeyPress:
			goto finish;
		default:
			break;
		}
	}

finish:
	eglTerminate(egl_display);
	XUnmapWindow(x11_display, win);
	XDestroyWindow(x11_display, win);
	XCloseDisplay(x11_display);
}
