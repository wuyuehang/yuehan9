#include <assert.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <sys/time.h>
#include "utility.h"
#include "warp_types.h"
#include <glm/glm.hpp>
#include <glm/gtx/transform.hpp>
#include <glm/gtx/rotate_vector.hpp>
#include <glm/gtc/matrix_transform.hpp>

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
	load_shader_from_file("shaders/es3_dual_vao.vert", &source);
	glShaderSource(vertex_shader, 1, (const GLchar * const*)&source, NULL);
	glCompileShader(vertex_shader);

	glGetShaderiv(vertex_shader, GL_COMPILE_STATUS, &status);
	if (!status) {
		glGetShaderInfoLog(vertex_shader, 512, NULL, message);
		printf("%s\n", message);
	}

	GLuint fragment_shader[2];
	fragment_shader[0] = glCreateShader(GL_FRAGMENT_SHADER);
	free(source);
	load_shader_from_file("shaders/es3_dual_vao.frag0", &source);
	glShaderSource(fragment_shader[0], 1, (const GLchar * const*)&source, NULL);
	glCompileShader(fragment_shader[0]);

	glGetShaderiv(fragment_shader[0], GL_COMPILE_STATUS, &status);
	if (!status) {
		glGetShaderInfoLog(fragment_shader[0], 512, NULL, message);
		printf("%s\n", message);
	}

	fragment_shader[1] = glCreateShader(GL_FRAGMENT_SHADER);
	free(source);
	load_shader_from_file("shaders/es3_dual_vao.frag1", &source);
	glShaderSource(fragment_shader[1], 1, (const GLchar * const*)&source, NULL);
	glCompileShader(fragment_shader[1]);

	glGetShaderiv(fragment_shader[1], GL_COMPILE_STATUS, &status);
	if (!status) {
		glGetShaderInfoLog(fragment_shader[1], 512, NULL, message);
		printf("%s\n", message);
	}

	GLuint program[2];
	program[0] = glCreateProgram();
	glAttachShader(program[0], vertex_shader);
	glAttachShader(program[0], fragment_shader[0]);
	glLinkProgram(program[0]);

	program[1] = glCreateProgram();
	glAttachShader(program[1], vertex_shader);
	glAttachShader(program[1], fragment_shader[1]);
	glLinkProgram(program[1]);

	GLuint mvp_uni[2];
	mvp_uni[0] = glGetUniformLocation(program[0], "mvp");
	mvp_uni[1] = glGetUniformLocation(program[1], "mvp");

	glm::mat4 view_mat = glm::lookAt(
			glm::vec3(-2, 2, 4),
			glm::vec3(0, 0, 0),
			glm::vec3(0, 1, 0)
	);

	glm::mat4 proj_mat = glm::perspective(glm::radians(60.0f), 1.0f, 0.01f, 100.0f);

	/* set up resource */

	GLfloat pos_buf[] = {
			// top face
			-0.5f, 0.5f, -0.5f,
			0.5f, 0.5f, -0.5f,
			-0.5f, 0.5f, 0.5f,

			-0.5f, 0.5f, 0.5f,
			0.5f, 0.5f, -0.5f,
			0.5f, 0.5f, 0.5f,

			// bottom face
			-0.5f, -0.5f, -0.5f,
			-0.5f, -0.5f, 0.5f,
			0.5f, -0.5f, -0.5f,

			0.5f, -0.5f, -0.5f,
			-0.5f, -0.5f, 0.5f,
			0.5f, -0.5f, 0.5f,

			// front face
			-0.5f, 0.5f, 0.5f,
			0.5f, 0.5f, 0.5f,
			-0.5f, -0.5f, 0.5f,

			-0.5f, -0.5f, 0.5f,
			0.5f, 0.5f, 0.5f,
			0.5f, -0.5f, 0.5f,

			// back face
			-0.5f, 0.5f, -0.5f,
			-0.5f, -0.5f, -0.5f,
			0.5f, 0.5f, -0.5f,

			0.5f, 0.5f, -0.5f,
			-0.5f, -0.5f, -0.5f,
			0.5f, -0.5f, -0.5f,

			// left face
			-0.5f, 0.5f, 0.5f,
			-0.5f, -0.5f, 0.5f,
			-0.5f, -0.5f, -0.5f,

			-0.5f, 0.5f, 0.5f,
			-0.5f, -0.5f, -0.5f,
			-0.5f, 0.5f, -0.5f,

			// right face
			0.5f, 0.5f, 0.5f,
			0.5f, -0.5f, -0.5f,
			0.5f, -0.5f, 0.5f,

			0.5f, 0.5f, 0.5f,
			0.5f, 0.5f, -0.5f,
			0.5f, -0.5f, -0.5f,
	};

	GLuint vao[2];
	glGenVertexArrays(2, vao);

	/* handle 1st vao for object box */
	glBindVertexArray(vao[0]);

	GLuint vbo_pos;
	glGenBuffers(1, &vbo_pos);
	glBindBuffer(GL_ARRAY_BUFFER, vbo_pos);
	glBufferData(GL_ARRAY_BUFFER, sizeof(pos_buf), pos_buf, GL_STATIC_DRAW);

	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), (GLvoid *)0);

	/* handle 2nd vao for light box */
	glBindVertexArray(vao[1]);

	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), (GLvoid *)0);

	/* game loop */
	while (1) {

		glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		struct timeval tv;
		gettimeofday(&tv, NULL);
		static long int startup = 0;
		glm::mat4 scale_mat, rotate_mat, translate_mat, world_mat, mvp_mat;

		if (0 == startup)
			startup = tv.tv_sec * 1000000 + tv.tv_usec;

		float gap = (tv.tv_sec * 1000000 + tv.tv_usec - startup) / 10000.0f;

		/* prepare matrix for drawing first orbit box */
		scale_mat = glm::scale(glm::vec3(0.5f, 0.5f, 0.5f));

		rotate_mat = glm::rotate(glm::mat4(1.0f), glm::radians(gap), glm::vec3(0.0f, 1.0f, 0.0f));

		translate_mat = glm::translate(glm::vec3(2.0f, 0.0f, 0.0f));

		world_mat = rotate_mat * translate_mat * scale_mat;

		mvp_mat = proj_mat * view_mat * world_mat;

		glBindVertexArray(vao[0]);
		glUseProgram(program[0]);
		glUniformMatrix4fv(mvp_uni[0], 1, GL_FALSE, &mvp_mat[0][0]);

		glDrawArrays(GL_TRIANGLES, 0, 3 * 36);
		glBindVertexArray(0);

		/* prepare matrix for drawing second rotate box */
		scale_mat = glm::scale(glm::vec3(1.0f, 1.0f, 1.0f));

		rotate_mat = glm::rotate(glm::mat4(1.0f), glm::radians(gap / 2.0f), glm::vec3(1.0f, 0.0f, 0.0f));

		world_mat = rotate_mat * scale_mat;

		mvp_mat = proj_mat * view_mat * world_mat;

		glBindVertexArray(vao[1]);
		glUseProgram(program[1]);
		glUniformMatrix4fv(mvp_uni[1], 1, GL_FALSE, &mvp_mat[0][0]);

		glDrawArrays(GL_TRIANGLES, 0, 3 * 36);
		glBindVertexArray(0);

		eglSwapBuffers(wrt->egl_display, wrt->egl_surface);

		while (XPending(wrt->x11_display)) {
			XNextEvent(wrt->x11_display, &(wrt->event));

			switch(wrt->event.type) {
				case ButtonPress:
				case KeyPress:
					goto finish;
				default:
					break;
			}
		}
	}

finish:
	glDeleteShader(vertex_shader);
	glDeleteShader(fragment_shader[0]);
	glDeleteShader(fragment_shader[1]);
	glDeleteProgram(program[0]);
	glDeleteProgram(program[1]);
	glDeleteBuffers(1, &vbo_pos);
	glDeleteVertexArrays(2, vao);
	destroy_warp_runtime(wrt);
	return 0;
}
