#include "warp_types.h"

int main()
{
	struct warp_runtime *wr = (struct warp_runtime *)malloc(sizeof(struct warp_runtime));

	create_warp_runtime(wr);

	GLfloat buffer[] = {
		1.f
	};

	glBindBuffer(GL_ARRAY_BUFFER, wr->vbo[0]);
	glBufferData(GL_ARRAY_BUFFER, sizeof(buffer), buffer, GL_DYNAMIC_DRAW);
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	EGLint ctx_attrib[] =
			{
				EGL_CONTEXT_CLIENT_VERSION, 3,
				EGL_NONE
			};

	EGLContext ctx2 = eglCreateContext(wr->egl_display, wr->egl_config, wr->egl_context, ctx_attrib);
	assert(ctx2 != EGL_NO_CONTEXT);

	eglMakeCurrent(wr->egl_display, wr->egl_surface, wr->egl_surface, ctx2);

	glBindBuffer(GL_ARRAY_BUFFER, wr->vbo[0]);
	int *ctx2_data = glMapBufferRange(GL_ARRAY_BUFFER, 0, sizeof(buffer), GL_MAP_READ_BIT);
	printf("%x\n", *ctx2_data);
	glUnmapBuffer(GL_ARRAY_BUFFER);
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	return 0;
}
