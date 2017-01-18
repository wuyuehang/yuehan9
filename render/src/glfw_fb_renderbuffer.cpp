#include "utility.h"
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>

extern GLboolean glewExperimental;

#define QUALD 64
#define QUALD_SQRT 8

GLfloat pos_buf[] = {
		-0.04f, 0.04f,
		0.04f, 0.04f,
		0.04f, -0.04f,
		-0.04f, 0.04f,
		0.04f, -0.04f,
		-0.04f, -0.04f,
};

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

	glEnable(GL_BLEND);
	glBlendFunc(GL_DST_ALPHA, GL_ONE_MINUS_DST_ALPHA);

	/* setup vao & vbo */
	GLuint vao;
	glGenVertexArrays(1, &vao);
	glBindVertexArray(vao);

	GLuint vbo_pos;
	glGenBuffers(1, &vbo_pos);
	glBindBuffer(GL_ARRAY_BUFFER, vbo_pos);
	glBufferData(GL_ARRAY_BUFFER, sizeof(pos_buf), pos_buf, GL_STATIC_DRAW);

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

	GLuint vbo_inst;
	glGenBuffers(1, &vbo_inst);
	glBindBuffer(GL_ARRAY_BUFFER, vbo_inst);
	glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec2) * QUALD, &go[0], GL_STATIC_DRAW);

	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(GLfloat), (GLvoid *)0);
	glVertexAttribDivisor(1, 1);

	/* setup shader & program */
	GLchar message[512];
	GLint status;
	GLchar *source = NULL;

	GLuint vertex_shader = glCreateShader(GL_VERTEX_SHADER);
	load_shader_from_file("shaders/glfw_instance_array.vert", &source);
	glShaderSource(vertex_shader, 1, &source, NULL);
	glCompileShader(vertex_shader);

	glGetShaderiv(vertex_shader, GL_COMPILE_STATUS, &status);
	if (!status) {
		glGetShaderInfoLog(vertex_shader, 512, NULL, message);
		printf("%s\n", message);
	}

	GLuint fragment_shader;
	fragment_shader = glCreateShader(GL_FRAGMENT_SHADER);
	free(source);
	load_shader_from_file("shaders/glfw_instance.frag", &source);
	glShaderSource(fragment_shader, 1, &source, NULL);
	glCompileShader(fragment_shader);

	glGetShaderiv(fragment_shader, GL_COMPILE_STATUS, &status);
	if (!status) {
		glGetShaderInfoLog(fragment_shader, 512, NULL, message);
		printf("%s\n", message);
	}
	free(source);

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

	/* create off-screen render buffer */
	GLuint rbo_color;
	glGenRenderbuffers(1, &rbo_color);
	glBindRenderbuffer(GL_RENDERBUFFER, rbo_color);
	glRenderbufferStorage(GL_RENDERBUFFER, GL_RGBA, 500, 500);

	GLuint rbo_depth;
	glGenRenderbuffers(1, &rbo_depth);
	glBindRenderbuffer(GL_RENDERBUFFER, rbo_depth);
	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT24, 500, 500);

	GLuint fbo;
	glGenFramebuffers(1, &fbo);
	glBindFramebuffer(GL_DRAW_FRAMEBUFFER, fbo);

	glFramebufferRenderbuffer(GL_DRAW_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_RENDERBUFFER, rbo_color);
	assert(GL_FRAMEBUFFER_COMPLETE == glCheckFramebufferStatus(GL_DRAW_FRAMEBUFFER));
	glFramebufferRenderbuffer(GL_DRAW_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, rbo_depth);
	assert(GL_FRAMEBUFFER_COMPLETE == glCheckFramebufferStatus(GL_DRAW_FRAMEBUFFER));

	/* render off-screen buffer */
	glBindFramebuffer(GL_DRAW_FRAMEBUFFER, fbo);

	glClearColor(0.35f, 0.25f, 0.45f, 0.5f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glDrawArraysInstanced(GL_TRIANGLES, 0, 6, QUALD);

	/* game loop */
	while (!glfwWindowShouldClose(win))
	{
		glfwPollEvents();

		glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);
		glBindFramebuffer(GL_READ_FRAMEBUFFER, fbo);

		glBlitFramebuffer(0, 0, 500, 500, 0, 0, 500, 500, GL_COLOR_BUFFER_BIT, GL_NEAREST);
		glfwSwapBuffers(win);
	}

	/* cleanup */
	glDeleteShader(vertex_shader);
	glDeleteShader(fragment_shader);
	glDeleteProgram(prog);
	glDeleteRenderbuffers(1, &rbo_color);
	glDeleteRenderbuffers(1, &rbo_depth);
	glDeleteFramebuffers(1, &fbo);
	glDeleteBuffers(1, &vbo_pos);
	glDeleteBuffers(1, &vbo_inst);
	glDeleteVertexArrays(1, &vao);
	glfwTerminate();

	return 0;
}
