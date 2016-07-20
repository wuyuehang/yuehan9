#include <stdio.h>
#include <string.h>
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
	XFontStruct *font;
	char *text = "Wooo!";
	int textx, texty, text_width;

	dpy = XOpenDisplay(NULL);
	if (NULL == dpy) {
		fprintf(stderr, "failed to connect xserver\n");
		return -1;
	}

	bgc = BlackPixel(dpy, DefaultScreen(dpy));

	win = XCreateSimpleWindow(dpy, DefaultRootWindow(dpy),
		x, y,
		width, height,
		0, 0, bgc);

	XSelectInput(dpy, win, ExposureMask | KeyPressMask | ButtonPressMask);

	XMapWindow(dpy, win);

	gc_value.foreground = WhitePixel(dpy, DefaultScreen(dpy));
	font = XLoadQueryFont(dpy, "fixed");
	gc_value.font = font->fid;
	pen = XCreateGC(dpy, win, GCFont | GCForeground, &gc_value);
	text_width = XTextWidth(font, text, strlen(text));

	while(1){
		XNextEvent(dpy, &event);
		switch(event.type){
		case Expose:
				textx = (width - text_width)/2;
				texty = (height + font->ascent)/2;
				XDrawString(dpy, event.xany.window, pen, textx, texty, text, strlen(text));
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
