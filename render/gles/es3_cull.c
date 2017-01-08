#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "utility.h"
#include "warp_types.h"

struct warp_runtime *wrt = NULL;

int main(int argc, char **argv)
{
	wrt = (struct warp_runtime *)malloc(sizeof(struct warp_runtime));
	assert(NULL != wrt);

	memset(wrt, 0, sizeof(struct warp_runtime));

	create_warp_runtime(wrt);

	glViewport(0, 0, 500, 500);

	/* setup shader & program */
	GLchar message[512];
	GLint status;
	GLchar *source = NULL;

	GLuint vertex_shader = glCreateShader(GL_VERTEX_SHADER);
	load_shader_from_file("shaders/es3_draw_rect.vert", &source);
	glShaderSource(vertex_shader, 1, (const GLchar * const*)&source, NULL);
	glCompileShader(vertex_shader);

	glGetShaderiv(vertex_shader, GL_COMPILE_STATUS, &status);
	if (!status) {
		glGetShaderInfoLog(vertex_shader, 512, NULL, message);
		printf("%s\n", message);
	}

	GLuint fragment_shader = glCreateShader(GL_FRAGMENT_SHADER);
	free(source);
	load_shader_from_file("shaders/es3_draw_rect.frag", &source);
	glShaderSource(fragment_shader, 1, (const GLchar * const*)&source, NULL);
	glCompileShader(fragment_shader);

	glGetShaderiv(fragment_shader, GL_COMPILE_STATUS, &status);
	if (!status) {
		glGetShaderInfoLog(fragment_shader, 512, NULL, message);
		printf("%s\n", message);
	}

	GLuint prog = glCreateProgram();
	glAttachShader(prog, vertex_shader);
	glAttachShader(prog, fragment_shader);
	glLinkProgram(prog);

	glGetProgramiv(prog, GL_LINK_STATUS, &status);
	if (!status) {
		glGetProgramInfoLog(prog, 512, NULL, message);
		printf("%s\n", message);
	}

	glUseProgram(prog);

	/* set up resource */

	/* packed in vec3 (pos) + vec4 (color) */
	GLfloat buffer[] = {
			/* down triangle in clock-wise direction */
			-0.25f, 0.25f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f,
			0.25f, -0.25f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f,
			-0.25f, -0.25f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f,
			/* up triangle  in counter clock-wise direction */
			-0.25f, 0.25f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f,
			0.25f, -0.25f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f,
			0.25f, 0.25f, 0.25f, 1.0f, 1.0f, 0.0f, 0.0f
	};

	GLuint vao;
	glGenVertexArrays(1, &vao);
	glBindVertexArray(vao);

	GLuint vbo;
	glGenBuffers(1, &vbo);
	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	glBufferData(GL_ARRAY_BUFFER, sizeof(buffer), buffer, GL_STATIC_DRAW);

	/* here we pack position and color buffer into a single
	 * vbo, therefore the offset should be considered when setting
	 * its relative attribute index on glsl side
	 */
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 7 * sizeof(GLfloat), (GLvoid *)0);

	glEnableVertexAttribArray(1);
	/* color buffer content is 3*sizeof(GLfloat) = 12
	 * offset from the vbo
	 */
	glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, 7 * sizeof(GLfloat), (GLvoid *)12);

	/* enable cull face, in following senario,
	 * only down triangle in cw direction is rendered
	 */
	glEnable(GL_CULL_FACE);
	glCullFace(GL_BACK);
	glFrontFace(GL_CW);

	/* game loop */
	while (1) {

		XNextEvent(wrt->x11_display, &(wrt->event));

		switch(wrt->event.type) {
		case Expose:
			/* do the render */
			glClearColor(0.3f, 0.3f, 0.3f, 1.0f);
			glClear(GL_COLOR_BUFFER_BIT);

			glBindVertexArray(vao);
			glDrawArrays(GL_TRIANGLES, 0, 6);
			glBindVertexArray(0);

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
	glDeleteShader(vertex_shader);
	glDeleteShader(fragment_shader);
	glDeleteProgram(prog);
	glDeleteBuffers(1, &vbo);
	glDeleteVertexArrays(1, &vao);
	destroy_warp_runtime(wrt);
	return 0;
}
