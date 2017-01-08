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
	load_shader_from_file("shaders/es3_draw_tri.vert", &source);
	glShaderSource(vertex_shader, 1, &source, NULL);
	glCompileShader(vertex_shader);

	glGetShaderiv(vertex_shader, GL_COMPILE_STATUS, &status);
	if (!status) {
		glGetShaderInfoLog(vertex_shader, 512, NULL, message);
		printf("%s\n", message);
	}

	GLuint fragment_shader = glCreateShader(GL_FRAGMENT_SHADER);
	free(source);
	load_shader_from_file("shaders/es3_draw_tri.frag", &source);
	glShaderSource(fragment_shader, 1, &source, NULL);
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
	GLfloat pos_buf[] = {
			-0.5f, -0.5f, 0.0f,
			 0.5f, -0.5f, 0.0f,
			 0.0f,  0.5f, 0.0f
	};

	GLuint vao;
	glGenVertexArrays(1, &vao);
	glBindVertexArray(vao);

	GLuint vbo_pos;
	glGenBuffers(1, &vbo_pos);
	glBindBuffer(GL_ARRAY_BUFFER, vbo_pos);
	glBufferData(GL_ARRAY_BUFFER, sizeof(pos_buf), pos_buf, GL_STATIC_DRAW);

	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), (GLvoid*)0);

	/* game loop */
	while (1) {

		XNextEvent(wrt->x11_display, &(wrt->event));

		switch(wrt->event.type) {
		case Expose:
			/* do the render */
			glClearColor(0.6f, 0.4f, 0.2f, 1.0f);
			glClear(GL_COLOR_BUFFER_BIT);

			glBindVertexArray(vao);
			glDrawArrays(GL_TRIANGLES, 0, 3);
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
	glDeleteBuffers(1, &vbo_pos);
	glDeleteVertexArrays(1, &vao);
	destroy_warp_runtime(wrt);
	return 0;
}
