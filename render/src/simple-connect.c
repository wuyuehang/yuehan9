#include <stdio.h>
#include <X11/Xlib.h>

/* start up a connection request */
static Display *dpy;

int main(int argc, char **argv)
{
	dpy = XOpenDisplay(NULL);
	if (NULL == dpy) {
		fprintf(stderr, "failed to connect xserver\n");
		return -1;
	}

	XCloseDisplay(dpy);
	dpy = NULL;
	return 0;
}
