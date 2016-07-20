#include <stdio.h>
#include <X11/Xlib.h>

Display *dpy;
char *dpy_name = NULL;
Window win;
XEvent event;
int dscrn; //store default screen index
int x = 0;
int y = 0;
unsigned long bgc;
unsigned int width = 500;
unsigned int height = 500;

int main(int argc, char **argv)
{
	GC pen;
	XGCValues gc_value;
	Colormap cmap;
	XColor xc, xc2;

	dpy = XOpenDisplay(NULL);
	if (NULL == dpy) {
		fprintf(stderr, "failed to connect xserver\n");
		return -1;
	}

	cmap = DefaultColormap(dpy, DefaultScreen(dpy));

	XAllocNamedColor(dpy, cmap, "Red", &xc, &xc2);

	bgc = xc.pixel;

	win = XCreateSimpleWindow(dpy, DefaultRootWindow(dpy),
		x, y,
		width, height,
		0, 0, bgc);

	XSelectInput(dpy, win, ExposureMask | KeyPressMask | ButtonPressMask);

	XMapWindow(dpy, win);

	gc_value.foreground = WhitePixel(dpy, DefaultScreen(dpy));
	gc_value.line_width = 6;
	gc_value.line_style = LineSolid;
	pen = XCreateGC(dpy, win, GCForeground | GCLineWidth | GCLineStyle, &gc_value);

	while(1){
		XNextEvent(dpy, &event);
		switch(event.type){
		case Expose:
			XDrawLine(dpy, win, pen, 0, 0, width, height);
			XDrawLine(dpy, win, pen, width, 0, 0, height);
			break;			  
		case ButtonPress:
		case KeyPress:
			goto finish;
		}
	}

finish:
	XUnmapWindow(dpy, win);
	XDestroyWindow(dpy, win);
	XCloseDisplay(dpy);
	dpy = NULL;
	return 0;
}
