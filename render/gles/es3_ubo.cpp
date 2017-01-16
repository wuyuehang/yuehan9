#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
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
	load_shader_from_file("shaders/es3_ubo.vert", &source);
	glShaderSource(vertex_shader, 1, (const GLchar * const*)&source, NULL);
	glCompileShader(vertex_shader);

	glGetShaderiv(vertex_shader, GL_COMPILE_STATUS, &status);
	if (!status) {
		glGetShaderInfoLog(vertex_shader, 512, NULL, message);
		printf("%s\n", message);
	}

	GLuint fragment_shader = glCreateShader(GL_FRAGMENT_SHADER);
	free(source);
	load_shader_from_file("shaders/es3_ubo.frag", &source);
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
	GLfloat pos_buf[] = {
			-0.25f, 0.25f, 0.0f,
			0.25f, -0.25f, 0.0f,
			-0.25f, -0.25f, 0.0f,
			0.25f, 0.25f, 0.0f
	};

	GLfloat color_buf[] = {
			1.0f, 0.0f, 0.0f, 0.0f,
			0.0f, 1.0f, 0.0f, 0.0f,
			0.0f, 0.0f, 1.0f, 0.0f,
			1.0f, 1.0f, 0.0f, 0.0f,
	};

	GLushort index_buf[] = {
			2, 0, 1, 3
	};

	GLuint vao;
	glGenVertexArrays(1, &vao);
	glBindVertexArray(vao);

	/* setup array buffer for position and color
	 */
	GLuint vbo_pos;
	glGenBuffers(1, &vbo_pos);
	glBindBuffer(GL_ARRAY_BUFFER, vbo_pos);
	glBufferData(GL_ARRAY_BUFFER, sizeof(pos_buf), pos_buf, GL_STATIC_DRAW);

	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), (GLvoid *)0);

	GLuint vbo_color;
	glGenBuffers(1, &vbo_color);
	glBindBuffer(GL_ARRAY_BUFFER, vbo_color);
	glBufferData(GL_ARRAY_BUFFER, sizeof(color_buf), color_buf, GL_STATIC_DRAW);

	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, 4 * sizeof(GLfloat), (GLvoid *)0);

	/* setup index buffer for index
	 */
	GLuint vbo_index;
	glGenBuffers(1, &vbo_index);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, vbo_index);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(index_buf), index_buf, GL_STATIC_DRAW);

	/* updating uniform block */
	glm::mat4 model_mat = glm::mat4(1.0f);

	glm::mat4 view_mat = glm::lookAt(
			glm::vec3(-2, 1, 2),
			glm::vec3(0, 0, 0),
			glm::vec3(0, 1, 0)
	);

	glm::mat4 proj_mat = glm::perspective(glm::radians(45.0f), 1.0f, 0.1f, 100.0f);

	GLuint ubo_blk_idx = glGetUniformBlockIndex(prog, "MVP");
	GLint ubo_sz;
	glGetActiveUniformBlockiv(prog, ubo_blk_idx, GL_UNIFORM_BLOCK_DATA_SIZE, &ubo_sz);

	const char *names[3] = {"uModel", "uView", "uProj"};
	GLuint indices[3];
	GLint size[3];
	GLint offset[3];
	GLint type[3];

	glGetUniformIndices(prog, 3, names, indices);
	glGetActiveUniformsiv(prog, 3, indices, GL_UNIFORM_OFFSET, offset);
	glGetActiveUniformsiv(prog, 3, indices, GL_UNIFORM_SIZE, size);
	glGetActiveUniformsiv(prog, 3, indices, GL_UNIFORM_TYPE, type);

	/* mat4 here is 4x4=16 GLfloat */
	GLubyte *buffer = (GLubyte *)malloc(ubo_sz);
	memcpy(buffer + offset[0], &model_mat[0][0], size[0] * sizeof(GLfloat) * 16);
	memcpy(buffer + offset[1], &view_mat[0][0], size[1] * sizeof(GLfloat) * 16);
	memcpy(buffer + offset[2], &proj_mat[0][0], size[2] * sizeof(GLfloat) * 16);

	GLuint ubo;
	glGenBuffers(1, &ubo);
	glBindBuffer(GL_UNIFORM_BUFFER, ubo);
	glBufferData(GL_UNIFORM_BUFFER, ubo_sz, buffer, GL_STATIC_DRAW);
	glBindBufferBase(GL_UNIFORM_BUFFER, ubo_blk_idx, ubo);

	/* game loop */
	while (1) {

		XNextEvent(wrt->x11_display, &(wrt->event));

		switch(wrt->event.type) {
		case Expose:
			/* do the render */
			glClearColor(0.3f, 0.3f, 0.3f, 1.0f);
			glClear(GL_COLOR_BUFFER_BIT);

			glBindVertexArray(vao);
			glDrawElements(GL_TRIANGLE_STRIP, 4, GL_UNSIGNED_SHORT, (void*)0);
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
	glDeleteBuffers(1, &vbo_pos);
	glDeleteBuffers(1, &vbo_color);
	glDeleteBuffers(1, &vbo_index);
	glDeleteBuffers(1, &ubo);
	glDeleteVertexArrays(1, &vao);
	glDeleteShader(vertex_shader);
	glDeleteShader(fragment_shader);
	glDeleteProgram(prog);

	destroy_warp_runtime(wrt);
	return 0;
}
