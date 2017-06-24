#ifndef _OGL_WARP_H_
#define _OGL_WARP_H_

#include <stdio.h>
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include "utility.h"

#define OGL_MAX_VBO	8
#define OGL_MAX_VAO	8
#define OGL_MAX_VS	8
#define OGL_MAX_FS	8
#define OGL_MAX_PROG	8
#define OGL_WIN_WIDTH	800
#define OGL_WIN_HEIGHT	800
#define _ogl_enable_debug_ 1

void dbg_callback(GLenum source, GLenum type, GLuint id,
		GLenum severity, GLsizei lenghth,
		const GLchar *message, const void *userParam)
{
	printf("(%d): %s\n", id, message);
}

static void ogl_error_cb(int error, const char* description)
{
	fputs(description, stderr);
	fputs("\n", stderr);
}

void ogl_key_cb(GLFWwindow* win, int key, int scancode, int action, int mode)
{
	if ((key & GLFW_KEY_ESCAPE) && action == GLFW_PRESS)
		glfwSetWindowShouldClose(win, GL_TRUE);
}

struct ogl_warp {
	GLFWwindow *win;
	// ogl
	GLuint vao[OGL_MAX_VAO];
	GLuint vbo[OGL_MAX_VBO];
	GLuint vertex_shaders[OGL_MAX_VBO];
	GLuint fragment_shaders[OGL_MAX_VBO];
	GLuint programs[OGL_MAX_PROG];
};

void create_ogl_warp(struct ogl_warp *ow)
{
	glfwSetErrorCallback(ogl_error_cb);

	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 5);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	glfwWindowHint(GLFW_RESIZABLE, GL_FALSE);
#if _ogl_enable_debug_
	glfwWindowHint(GLFW_OPENGL_DEBUG_CONTEXT, GL_TRUE);
#endif

	ow->win = glfwCreateWindow(OGL_WIN_WIDTH, OGL_WIN_HEIGHT, __FILE__, NULL, NULL);

	glfwMakeContextCurrent(ow->win);

	glfwSetKeyCallback(ow->win, ogl_key_cb);

	glewExperimental = GL_TRUE;
	glewInit();

#if _ogl_enable_debug_
	glEnable(GL_DEBUG_OUTPUT);
	glDebugMessageCallback(dbg_callback, NULL);
	glDebugMessageControl(GL_DONT_CARE, GL_DONT_CARE,
			GL_DONT_CARE, 0, NULL, GL_TRUE);
#endif
	glViewport(0, 0, OGL_WIN_WIDTH, OGL_WIN_HEIGHT);

	glGenVertexArrays(OGL_MAX_VAO, ow->vao);
	glGenBuffers(OGL_MAX_VBO, ow->vbo);
}

void create_ogl_warp_shaders(GLuint *vidx, char *vpath, GLuint *fidx, char *fpath)
{
	GLchar message[512];
	char *source;
	GLint status;

	if (vidx != NULL) {
		*vidx = glCreateShader(GL_VERTEX_SHADER);
		load_shader_from_file(vpath, &source);
		glShaderSource(*vidx, 1, &source, NULL);
		glCompileShader(*vidx);

		glGetShaderiv(*vidx, GL_COMPILE_STATUS, &status);
		if (!status) {
			glGetShaderInfoLog(*vidx, 512, NULL, message);
			printf("%s\n", message);
		}
		free(source);
	}

	if (fidx != NULL) {
		*fidx = glCreateShader(GL_FRAGMENT_SHADER);
		load_shader_from_file(fpath, &source);
		glShaderSource(*fidx, 1, &source, NULL);
		glCompileShader(*fidx);

		glGetShaderiv(*fidx, GL_COMPILE_STATUS, &status);
		if (!status) {
			glGetShaderInfoLog(*fidx, 512, NULL, message);
			printf("%s\n", message);
		}
		free(source);
	}
}

void create_ogl_warp_program(GLuint vidx, GLuint fidx, GLuint *prog)
{
	GLchar message[512];
	GLint status;

	*prog = glCreateProgram();
	glAttachShader(*prog, vidx);
	glAttachShader(*prog, fidx);
	glLinkProgram(*prog);

	glGetProgramiv(*prog, GL_LINK_STATUS, &status);
	if (!status) {
		glGetProgramInfoLog(*prog, 512, NULL, message);
		printf("%s\n", message);
	}

	glUseProgram(*prog);
}
#endif
