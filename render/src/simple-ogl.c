#include <X11/Xlib.h>
#include <GL/glx.h>

static int attributeList[] = { GLX_RGBA, GLX_DOUBLEBUFFER, None };

/* draw a red rectangle */
static void redraw(Display *dpy, Window win)
{
	glClear(GL_COLOR_BUFFER_BIT);
	glColor3f(1.0, 0.0, 0.0);
	glRectf(-0.5, -0.5, 0.5, 0.5);
	glXSwapBuffers(dpy, win);
}

/* gcc -gdwarf-2 -g3 -O0 -o simple simple.c -lGL -lX11 */
int main(int argc, char **argv)
{
	Display *dpy;
	XVisualInfo *vi;
	GLXContext ctx;
	Window win;
	XEvent event;
	int x = 0;
	int y = 0;
	unsigned int width = 500;
	unsigned int height = 500;
	unsigned long bgc;
	int dummy;

	/* connect display */
	dpy = XOpenDisplay(NULL);

	glXQueryExtension(dpy, &dummy, &dummy);

	vi = glXChooseVisual(dpy, DefaultScreen(dpy), attributeList);

	ctx = glXCreateContext(dpy, vi, 0, GL_TRUE);

	bgc = BlackPixel(dpy, DefaultScreen(dpy));

	/* create a black window */
	win = XCreateSimpleWindow(dpy, DefaultRootWindow(dpy), x, y,
			width, height,
			0, 0,
			bgc);

	XSelectInput(dpy, win, ExposureMask | KeyPressMask | ButtonPressMask);

	XMapWindow(dpy, win);

	/* bind GL context to current window */
	glXMakeCurrent(dpy, win, ctx);

	while(1){
		XNextEvent(dpy, &event);
		switch(event.type){
		case Expose:
			redraw(dpy, win);
			break;			  
		case ButtonPress:
		case KeyPress:
			goto finish;
		}
	}

finish:

	glXDestroyContext(dpy, ctx);
	XUnmapWindow(dpy, win);
	XDestroyWindow(dpy, win);
	XCloseDisplay(dpy);
	dpy = NULL;

	return 0;
}
