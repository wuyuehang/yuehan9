#include "utility.h"
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <SOIL/SOIL.h>

extern GLboolean glewExperimental;

#define QUALD 64
#define QUALD_SQRT 8

GLfloat pos_buf_pass1[] = {
		-0.04f, 0.04f,
		0.04f, 0.04f,
		0.04f, -0.04f,
		-0.04f, 0.04f,
		0.04f, -0.04f,
		-0.04f, -0.04f,
};

GLfloat pos_buf_pass2[] = {
		-0.25f, 0.25f, 0.0f,
		0.25f, -0.25f, 0.0f,
		-0.25f, -0.25f, 0.0f,
		0.25f, 0.25f, 0.0f
};

GLfloat texcord_buf_pass2[] = {
		0.0f, 0.0f,
		1.0f, 1.0f,
		0.0f, 1.0f,
		1.0f, 0.0f,
};

GLushort index_buf_pass2[] = {
		0, 1, 2,
		0, 3, 1
};

GLchar message[512];
GLint status;
GLchar *source = NULL;

void key_cb(GLFWwindow* win, int key, int scancode, int action, int mode)
{
	if (key & (GLFW_KEY_Q | GLFW_KEY_ESCAPE) && action == GLFW_PRESS)
		glfwSetWindowShouldClose(win, GL_TRUE);
}

int main(int argc, char **argv)
{
	glfwInit();

	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	GLFWwindow* win = glfwCreateWindow(500, 500, __FILE__, NULL, NULL);
	glfwMakeContextCurrent(win);
	glfwSetKeyCallback(win, key_cb);

	glewExperimental = GL_TRUE;
	glewInit();

	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LESS);
	glViewport(0, 0, 500, 500);

	/* setup vao & vbo for pass1 render to texture */
	GLuint vao_pass1;
	glGenVertexArrays(1, &vao_pass1);
	glBindVertexArray(vao_pass1);

	GLuint vbo_pos_pass1;
	glGenBuffers(1, &vbo_pos_pass1);
	glBindBuffer(GL_ARRAY_BUFFER, vbo_pos_pass1);
	glBufferData(GL_ARRAY_BUFFER, sizeof(pos_buf_pass1), pos_buf_pass1, GL_STATIC_DRAW);

	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(GLfloat), (GLvoid *)0);

	glm::vec2 go[QUALD];
	int index = 0;
	GLfloat rate = -0.90;

	for (int i = 0; i < QUALD_SQRT; i++) {
		for (int j = 0; j < QUALD_SQRT; j++) {
			glm::vec2 move;
			move.x = (GLfloat)i * (2.0f / QUALD_SQRT) + rate;
			move.y = (GLfloat)j * (2.0f / QUALD_SQRT) + rate;
			go[index++] = move;
		}
	}

	GLuint vbo_inst_pass1;
	glGenBuffers(1, &vbo_inst_pass1);
	glBindBuffer(GL_ARRAY_BUFFER, vbo_inst_pass1);
	glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec2) * QUALD, &go[0], GL_STATIC_DRAW);

	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(GLfloat), (GLvoid *)0);
	glVertexAttribDivisor(1, 1);

	/* setup shader & program  for pass1 */
	GLchar message[512];
	GLint status;
	GLchar *source = NULL;

	GLuint vertex_shader_pass1 = glCreateShader(GL_VERTEX_SHADER);
	load_shader_from_file("shaders/glfw_fb_rendertex_stage1.vert", &source);
	glShaderSource(vertex_shader_pass1, 1, &source, NULL);
	glCompileShader(vertex_shader_pass1);

	glGetShaderiv(vertex_shader_pass1, GL_COMPILE_STATUS, &status);
	if (!status) {
		glGetShaderInfoLog(vertex_shader_pass1, 512, NULL, message);
		printf("%s\n", message);
	}

	GLuint fragment_shader_pass1;
	fragment_shader_pass1 = glCreateShader(GL_FRAGMENT_SHADER);
	free(source);
	load_shader_from_file("shaders/glfw_fb_rendertex_stage1.frag", &source);
	glShaderSource(fragment_shader_pass1, 1, &source, NULL);
	glCompileShader(fragment_shader_pass1);

	glGetShaderiv(fragment_shader_pass1, GL_COMPILE_STATUS, &status);
	if (!status) {
		glGetShaderInfoLog(fragment_shader_pass1, 512, NULL, message);
		printf("%s\n", message);
	}
	free(source);

	GLuint prog_pass1 = glCreateProgram();
	glAttachShader(prog_pass1, vertex_shader_pass1);
	glAttachShader(prog_pass1, fragment_shader_pass1);
	glLinkProgram(prog_pass1);

	glGetProgramiv(prog_pass1, GL_LINK_STATUS, &status);
	if (!status) {
		glGetProgramInfoLog(prog_pass1, 512, NULL, message);
		printf("%s\n", message);
	}

	glUseProgram(prog_pass1);

	/* create off-screen texture */
	GLuint color_texture_pass1;
	glGenTextures(1, &color_texture_pass1);
	glBindTexture(GL_TEXTURE_2D, color_texture_pass1);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, 500, 500, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	GLuint depth_texture_pass1;
	glGenTextures(1, &depth_texture_pass1);
	glBindTexture(GL_TEXTURE_2D, depth_texture_pass1);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, 500, 500, 0, GL_DEPTH_COMPONENT, GL_UNSIGNED_BYTE, NULL);

	GLuint fbo;
	glGenFramebuffers(1, &fbo);
	glBindFramebuffer(GL_FRAMEBUFFER, fbo);

	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, color_texture_pass1, 0);
	assert(GL_FRAMEBUFFER_COMPLETE == glCheckFramebufferStatus(GL_FRAMEBUFFER));
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, depth_texture_pass1, 0);
	assert(GL_FRAMEBUFFER_COMPLETE == glCheckFramebufferStatus(GL_FRAMEBUFFER));

	/* render off-screen buffer */
	glBindFramebuffer(GL_DRAW_FRAMEBUFFER, fbo);

	glClearColor(0.35f, 0.25f, 0.45f, 0.5f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glDrawArraysInstanced(GL_TRIANGLES, 0, 6, QUALD);

	/* setup second pass which using resoult from pass1 as texture */

	GLuint vao_pass2;
	glGenVertexArrays(1, &vao_pass2);
	glBindVertexArray(vao_pass2);

	GLuint vbo_pos_pass2;
	glGenBuffers(1, &vbo_pos_pass2);
	glBindBuffer(GL_ARRAY_BUFFER, vbo_pos_pass2);
	glBufferData(GL_ARRAY_BUFFER, sizeof(pos_buf_pass2), pos_buf_pass2, GL_STATIC_DRAW);

	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), (GLvoid *)0);

	GLuint vbo_texcord_pass2;
	glGenBuffers(1, &vbo_texcord_pass2);
	glBindBuffer(GL_ARRAY_BUFFER, vbo_texcord_pass2);
	glBufferData(GL_ARRAY_BUFFER, sizeof(texcord_buf_pass2), texcord_buf_pass2, GL_STATIC_DRAW);

	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(GLfloat), (GLvoid *)0);

	GLuint vbo_index_pass2;
	glGenBuffers(1, &vbo_index_pass2);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, vbo_index_pass2);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(index_buf_pass2), index_buf_pass2, GL_STATIC_DRAW);

	/* setup shader */
	GLuint vertex_shader_pass2 = glCreateShader(GL_VERTEX_SHADER);
	load_shader_from_file("shaders/glfw_fb_rendertex_stage2.vert", &source);
	glShaderSource(vertex_shader_pass2, 1, &source, NULL);
	glCompileShader(vertex_shader_pass2);

	glGetShaderiv(vertex_shader_pass2, GL_COMPILE_STATUS, &status);
	if (!status) {
		glGetShaderInfoLog(vertex_shader_pass2, 512, NULL, message);
		printf("%s\n", message);
	}

	GLuint fragment_shader_pass2 = glCreateShader(GL_FRAGMENT_SHADER);
	free(source);
	load_shader_from_file("shaders/glfw_fb_rendertex_stage2.frag", &source);
	glShaderSource(fragment_shader_pass2, 1, &source, NULL);
	glCompileShader(fragment_shader_pass2);

	glGetShaderiv(fragment_shader_pass2, GL_COMPILE_STATUS, &status);
	if (!status) {
		glGetShaderInfoLog(fragment_shader_pass2, 512, NULL, message);
		printf("%s\n", message);
	}

	GLuint prog_pass2 = glCreateProgram();
	glAttachShader(prog_pass2, vertex_shader_pass2);
	glAttachShader(prog_pass2, fragment_shader_pass2);
	glLinkProgram(prog_pass2);
	glUseProgram(prog_pass2);

	glGetProgramiv(prog_pass2, GL_LINK_STATUS, &status);
	if (!status) {
		glGetProgramInfoLog(prog_pass2, 512, NULL, message);
		printf("%s\n", message);
	}

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, color_texture_pass1);

	glUniform1i(glGetUniformLocation(prog_pass2, "v4v_tex2d"), 0);

	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	/* game loop */
	while (!glfwWindowShouldClose(win))
	{
		glfwPollEvents();

		glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
		glClearDepth(1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_SHORT, (void*)0);

		glfwSwapBuffers(win);
	}

	/* cleanup */
	glDeleteShader(vertex_shader_pass1);
	glDeleteShader(vertex_shader_pass2);
	glDeleteShader(fragment_shader_pass1);
	glDeleteShader(fragment_shader_pass2);
	glDeleteProgram(prog_pass1);
	glDeleteProgram(prog_pass2);

	glDeleteTextures(1, &color_texture_pass1);
	glDeleteTextures(1, &depth_texture_pass1);
	glDeleteFramebuffers(1, &fbo);

	glDeleteBuffers(1, &vbo_pos_pass1);
	glDeleteBuffers(1, &vbo_inst_pass1);
	glDeleteVertexArrays(1, &vao_pass1);

	glDeleteBuffers(1, &vbo_pos_pass2);
	glDeleteBuffers(1, &vbo_texcord_pass2);
	glDeleteBuffers(1, &vbo_index_pass2);
	glDeleteVertexArrays(1, &vao_pass2);
	glfwTerminate();

	return 0;
}
