#include <assert.h>
#include <stdlib.h>
#include <string.h>
#include "warp_types.h"

struct warp_runtime *wrt = NULL;

int main(int argc, char **argv)
{
	wrt = (struct warp_runtime *)malloc(sizeof(struct warp_runtime));
	assert(NULL != wrt);

	memset(wrt, 0, sizeof(struct warp_runtime));

	create_warp_runtime(wrt);

	glViewport(0, 0, 500, 500);

	while (1) {

		XNextEvent(wrt->x11_display, &(wrt->event));

		switch(wrt->event.type) {
		case Expose:
			glClearColor(1.0f, 0.0f, 0.0f, 0.0f);
			glClear(GL_COLOR_BUFFER_BIT);
			eglSwapBuffers(wrt->egl_display, wrt->egl_surface);
			break;
		case ButtonPress:
		case KeyPress:
			goto finish;
		default:
			break;
		}
	}

finish:
	destroy_warp_runtime(wrt);
	return 0;
}
