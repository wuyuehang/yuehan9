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
unsigned long fgc;
unsigned int width = 500;
unsigned int height = 500;

void info(void)
{
	dscrn = DefaultScreen(dpy);

	if (dpy_name = XDisplayName(NULL))
		fprintf(stdout, "display name: %s\n", dpy_name);

	fprintf(stdout, "width: %u, height: %u\n", DisplayWidth(dpy, dscrn), DisplayHeight(dpy, dscrn));
	fprintf(stdout, "width: %umm, height: %umm\n", DisplayWidthMM(dpy, dscrn), DisplayHeightMM(dpy, dscrn));
	fprintf(stdout, "X protocol %d.%d\n", ProtocolVersion(dpy), ProtocolRevision(dpy));
	fprintf(stdout, "server vendor: %s\n", ServerVendor(dpy));
	fprintf(stdout, "root depth %d\n", DisplayPlanes(dpy, dscrn));
	fprintf(stdout, "display name: %s\n", DisplayString(dpy));
}

int main(int argc, char **argv)
{
	dpy = XOpenDisplay(NULL);
	if (NULL == dpy) {
		fprintf(stderr, "failed to connect xserver\n");
		return -1;
	}

	bgc = WhitePixel(dpy, DefaultScreen(dpy));

	win = XCreateSimpleWindow(dpy, DefaultRootWindow(dpy),
		x, y,
		width, height,
		0, 0, bgc);

	XSelectInput(dpy, win, ExposureMask | KeyPressMask | ButtonPressMask);

	XMapWindow(dpy, win);

	info();

	while(1){
		XNextEvent(dpy, &event);
		switch(event.type){
		case Expose:
			XFillRectangle(dpy, win, DefaultGC(dpy, dscrn), 20, 20, 10, 10);
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
