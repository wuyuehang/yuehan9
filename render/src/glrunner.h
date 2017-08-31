#ifndef _GLRUNNER_H_
#define _GLRUNNER_H_

#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <cassert>
#include <iostream>
#include <fstream>

#define GR_WIDTH	1024
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
		this->_w = GR_WIDTH;
		this->_h = GR_HEIGHT;
		InitWindonw();
		InitGL();
		InitQuadDrawPipe();
	}
	GlRunner(frameCallBack fcb, int w, int h) {
		assert(fcb);
		assert(w > 0);
		assert(h > 0);
		this->_renderCB = fcb;
		this->_w = w;
		this->_h = h;
		InitWindonw();
		InitGL();
		InitQuadDrawPipe();
	}
	~GlRunner();
	static void OnKeyboard(GLFWwindow*, int, int, int, int);
	GLuint BuildShaderProgram(const char *filename, GLenum type);
	GLuint BuildProgramPipeline();
	GLuint BuildXfb(const char *filename, GLsizei count, const char **varyings, GLenum mode);
	void UpdateKeyboardCB(GLFWkeyfun);
	void OnRender();
	void DrawQuad(GLuint);
	GLFWwindow* win() {
		return this->_win;
	}

private:
	void InitWindonw();
	void InitGL();
	void InitQuadDrawPipe();
	void ReadFile(const char *filename, std::string& source);
	GLFWwindow *_win;
	int _w;
	int _h;
	frameCallBack _renderCB;
	// quad
	GLuint _quadVS;
	GLuint _quadFS;
	GLuint _quadPPO;
	GLuint _quadVAO;
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

	this->_win = glfwCreateWindow(this->_w, this->_h, __FILE__, nullptr, nullptr);

	glfwMakeContextCurrent(this->_win);

	glfwSetKeyCallback(this->_win, this->OnKeyboard);

	glewExperimental = GL_TRUE;
	glewInit();

#if GR_DEBUG
	glEnable(GL_DEBUG_OUTPUT);
	glDebugMessageCallback(dbgCB, nullptr);
	glDebugMessageControl(GL_DONT_CARE, GL_DONT_CARE, GL_DONT_CARE, 0, nullptr, GL_TRUE);
#endif
}

void GlRunner::InitGL()
{
	glViewport(0, 0, this->_w, this->_h);
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LESS);
}

void GlRunner::InitQuadDrawPipe()
{
	_quadVS = BuildShaderProgram("shaders/quad.vert", GL_VERTEX_SHADER);
	_quadFS = BuildShaderProgram("shaders/quad.frag", GL_FRAGMENT_SHADER);
	_quadPPO = BuildProgramPipeline();

	// draw quad
	GLfloat pos_buf[] = {
			-0.9f, 0.9f,
			0.9f, -0.9f,
			-0.9f, -0.9f,
			0.9f, 0.9f
	};

	GLfloat texcord_buf[] = {
			0.0f, 0.0f,
			1.0f, 1.0f,
			0.0f, 1.0f,
			1.0f, 0.0f
	};

	GLushort index_buf[] = {
			0, 1, 2,
			0, 1, 3
	};

	GLuint VBO[3];
	glGenVertexArrays(1, &_quadVAO);
	glBindVertexArray(_quadVAO);

	glGenBuffers(3, VBO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO[0]);
	glBufferData(GL_ARRAY_BUFFER, sizeof(pos_buf), pos_buf, GL_STATIC_DRAW);
	glBindBuffer(GL_ARRAY_BUFFER, VBO[1]);
	glBufferData(GL_ARRAY_BUFFER, sizeof(texcord_buf), texcord_buf, GL_STATIC_DRAW);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, VBO[2]);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(index_buf), index_buf, GL_STATIC_DRAW);

	enum {attr_pos, attr_texc};
	glEnableVertexAttribArray(attr_pos);
	glVertexAttribFormat(attr_pos, 2, GL_FLOAT, GL_FALSE, 0);
	glVertexAttribBinding(attr_pos, 0);
	glBindVertexBuffer(attr_pos, VBO[0], 0, 2*sizeof(GLfloat));

	glEnableVertexAttribArray(attr_texc);
	glVertexAttribFormat(attr_texc, 2, GL_FLOAT, GL_FALSE, 0);
	glVertexAttribBinding(attr_texc, 1);
	glBindVertexBuffer(attr_texc, VBO[1], 0, 2*sizeof(GLfloat));

	glBindVertexArray(0);
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

void GlRunner::DrawQuad(GLuint texobj)
{
	glBindProgramPipeline(_quadPPO);
	glUseProgramStages(_quadPPO, GL_VERTEX_SHADER_BIT, _quadVS);
	glUseProgramStages(_quadPPO, GL_FRAGMENT_SHADER_BIT, _quadFS);

	glBindTexture(GL_TEXTURE_2D, texobj);
	glActiveTexture(GL_TEXTURE0);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

	glProgramUniform1i(_quadFS, glGetUniformLocation(_quadFS, "color2D"), 0);

	glBindVertexArray(_quadVAO);
	glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_SHORT, (void *)0);
	glBindVertexArray(0);
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

GLuint GlRunner::BuildXfb(const char *filename, GLsizei count, const char **varyings, GLenum mode)
{
	GLuint ShaderObj;
	GLuint ProgramObj;
	std::string source;
	GLchar message[512];
	GLint status;

	assert(filename);

	ReadFile(filename, source);

	const GLchar *token = source.c_str();

	ShaderObj = glCreateShader(GL_VERTEX_SHADER);

	glShaderSource(ShaderObj, 1, &token, nullptr);

	glCompileShader(ShaderObj);

	glGetShaderiv(ShaderObj, GL_COMPILE_STATUS, &status);

	if (!status) {
		glGetShaderInfoLog(ShaderObj, 512, nullptr, message);
		std::cout << message << std::endl;
	}

	ProgramObj = glCreateProgram();

	glAttachShader(ProgramObj, ShaderObj);

	glTransformFeedbackVaryings(ProgramObj, count, varyings, mode);


	glLinkProgram(ProgramObj);

	glGetProgramiv(ProgramObj, GL_LINK_STATUS, &status);
	if (!status) {
		glGetProgramInfoLog(ProgramObj, 512, nullptr, message);
		std::cout << message << std::endl;
	}

	glUseProgram(ProgramObj);

	return ProgramObj;
}
#endif
