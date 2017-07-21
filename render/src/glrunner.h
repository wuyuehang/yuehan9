#ifndef _GLRUNNER_H_
#define _GLRUNNER_H_

#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <cassert>
#include <iostream>
#include <fstream>

#define GR_WIDTH	1280
#define GR_HEIGHT	1024
#define GR_DEBUG	1

class GlRunner;

// every app should implement its one frame render
typedef void (*frameCallBack)(GlRunner *);

// opengl debug call should not aggrevate it to class object
void dbgCB(GLenum source, GLenum type, GLuint id, GLenum severity, GLsizei lenghth,
		const GLchar *message, const void *userParam);

// manifest for window create and opengl context operation
class GlRunner
{
public:
	GlRunner(frameCallBack fcb) {
		assert(fcb);
		this->_renderCB = fcb;
		InitWindonw();
		InitGL();
	}
	~GlRunner();
	static void OnKeyboard(GLFWwindow*, int, int, int, int);
	GLuint BuildShaderProgram(const char *filename, GLenum type);
	GLuint BuildProgramPipeline();
	void UpdateKeyboardCB(GLFWkeyfun);
	void OnRender();

private:
	void InitWindonw();
	void InitGL();
	void ReadFile(const char *filename, std::string& source);
	GLFWwindow *_win;
	frameCallBack _renderCB;
};

void dbgCB(GLenum source, GLenum type, GLuint id,
		GLenum severity, GLsizei lenghth,
		const GLchar *message, const void *userParam)
{
	std::cout <<"(" << id << "): " << message << std::endl;
}

void GlRunner::OnKeyboard(GLFWwindow* win, int key, int scancode, int action, int mode)
{
	if ((key == GLFW_KEY_ESCAPE) && action == GLFW_PRESS)
		glfwSetWindowShouldClose(win, GL_TRUE);
}

void GlRunner::UpdateKeyboardCB(GLFWkeyfun kbf)
{
	glfwSetKeyCallback(this->_win, kbf);
}

void GlRunner::InitWindonw()
{
	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 5);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	glfwWindowHint(GLFW_RESIZABLE, GL_FALSE);
	glfwWindowHint(GLFW_VISIBLE, GL_FALSE);

#if GR_DEBUG
	glfwWindowHint(GLFW_OPENGL_DEBUG_CONTEXT, GL_TRUE);
#endif

	this->_win = glfwCreateWindow(GR_WIDTH, GR_HEIGHT, __FILE__, nullptr, nullptr);

	glfwMakeContextCurrent(this->_win);

	glfwSetKeyCallback(this->_win, this->OnKeyboard);

	glewExperimental = GL_TRUE;
	glewInit();

#if GR_DEBUG
	glEnable(GL_DEBUG_OUTPUT);
	glDebugMessageCallback(dbgCB, NULL);
	glDebugMessageControl(GL_DONT_CARE, GL_DONT_CARE, GL_DONT_CARE, 0, NULL, GL_TRUE);
#endif
}

void GlRunner::InitGL()
{
	glViewport(0, 0, GR_WIDTH, GR_HEIGHT);
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LESS);
}

void GlRunner::OnRender()
{
	assert(this->_renderCB);

	glfwShowWindow(this->_win);

	while (!glfwWindowShouldClose(this->_win))
	{
		glfwPollEvents();
		_renderCB(this);
		glfwSwapBuffers(this->_win);
	}

	glfwTerminate();
}

void GlRunner::ReadFile(const char *pfile, std::string& source)
{
	std::string line;

	std::ifstream fhandle(pfile);
	assert(fhandle.is_open());

	while (getline(fhandle, line)) {
		source.append(line);
		source.append(1, '\n');
	}

	fhandle.close();

	return;
}

GLuint GlRunner::BuildShaderProgram(const char *filename, GLenum type)
{
	GLuint ShaderObj;
	std::string source;

	assert(filename);

	ReadFile(filename, source);

	const GLchar *token = source.c_str();

	ShaderObj = glCreateShaderProgramv(type, 1, &token);

	return ShaderObj;
}

GLuint GlRunner::BuildProgramPipeline()
{
	GLuint ppline;
	glGenProgramPipelines(1, &ppline);
	glBindProgramPipeline(ppline);

	return ppline;
}
#endif
