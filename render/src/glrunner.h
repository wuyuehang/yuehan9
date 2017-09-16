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
	void LogProgramInputAttribs(GLuint prog);
	void LogProgramUniform(GLuint prog);
	void LogProgramUniformBlock(GLuint prog);
	GLuint BuildProgramPipeline();
	GLuint BuildXfb(const char *filename, GLsizei count, const char **varyings, GLenum mode);
	void UpdateKeyboardCB(GLFWkeyfun);
	void OnRender();
	void DrawQuad(GLuint);
	const char* ConvertTypeToString(GLenum type);
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

	std::cout << std::endl << filename << std::endl;

	LogProgramInputAttribs(ShaderObj);

	LogProgramUniform(ShaderObj);

	LogProgramUniformBlock(ShaderObj);

	return ShaderObj;
}

void GlRunner::LogProgramInputAttribs(GLuint prog)
{
	GLint numAttirb;
	GLenum properties[3] = {
		GL_NAME_LENGTH,
		GL_TYPE,
		GL_LOCATION
	};

	glGetProgramInterfaceiv(prog, GL_PROGRAM_INPUT, GL_ACTIVE_RESOURCES, &numAttirb);

	std::cout << "\nInput Attributes: " << numAttirb << std::endl;

	for (int i = 0; i < numAttirb; i++) {

		GLint result[3];

		glGetProgramResourceiv(prog,
				GL_PROGRAM_INPUT, // program interface
				i, // attribute index
				3, // property count
				properties,
				3, // bufsize
				nullptr, // ?
				result);

		GLint namelength = result[0];
		char *attrib_name = new char[namelength];

		glGetProgramResourceName(prog,
				GL_PROGRAM_INPUT, // program interface
				i, // attribute index
				namelength, // bufsize
				nullptr, // length in fact
				attrib_name);

		std::cout << "name (" << attrib_name << ")" << std::endl;
		std::cout << "location(" << result[2] << ") type(" << ConvertTypeToString(result[1]) << ")" << std::endl;
		delete [] attrib_name;
	}
}

void GlRunner::LogProgramUniform(GLuint prog)
{
	GLint numUniform;
	GLenum properties[] = {
		GL_NAME_LENGTH,
		GL_TYPE,
		GL_LOCATION,
		GL_BLOCK_INDEX
	};

	glGetProgramInterfaceiv(prog, GL_UNIFORM, GL_ACTIVE_RESOURCES, &numUniform);

	std::cout << "\nUniform: " << numUniform << std::endl;

	for (int i = 0; i < numUniform; i++) {

		GLint result[4];

		glGetProgramResourceiv(prog,
				GL_UNIFORM, // program interface
				i, // uniform index
				4, // property count
				properties,
				4, // bufsize
				nullptr, // ?
				result);

		GLint namelength = result[0];
		char *uniform_name = new char[namelength];

		glGetProgramResourceName(prog,
				GL_UNIFORM, // program interface
				i, // uniform index
				namelength, // bufsize
				nullptr, // length in fact
				uniform_name);

		std::cout << "name(" << uniform_name << ")" << std::endl;
		std::cout << "block(" << result[3] << ") location(" << result[2] << ") type(" << ConvertTypeToString(result[1]) << ")" << std::endl;
		delete [] uniform_name;
	}
}

void GlRunner::LogProgramUniformBlock(GLuint prog)
{
	GLint numUniformBlock;

	GLenum blockPropties[] = {
		GL_NUM_ACTIVE_VARIABLES, // how many variables inside a block
		GL_NAME_LENGTH // named block or anomynous block name length
	};

	glGetProgramInterfaceiv(prog, GL_UNIFORM_BLOCK, GL_ACTIVE_RESOURCES, &numUniformBlock);

	std::cout << "\nUniform Block: " << numUniformBlock << std::endl;

	for (int i = 0; i < numUniformBlock; i++) {

		/* get block resource information first */
		GLint blockinfo[2];

		glGetProgramResourceiv(prog,
				GL_UNIFORM_BLOCK, // program interface
				i, // uniform block index
				2, // property count
				blockPropties,
				2, // bufsize
				nullptr, // ?
				blockinfo);

		char *uniformblock_name = new char[blockinfo[1]];

		glGetProgramResourceName(prog,
				GL_UNIFORM_BLOCK, // program interface
				i, // uniform block index
				blockinfo[1], // bufsize
				nullptr, // length in fact
				uniformblock_name);

		std::cout << "name(" << uniformblock_name << ") variables(" << blockinfo[0] << ")" << std::endl;

		delete [] uniformblock_name;
	}
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

const char* GlRunner::ConvertTypeToString(GLenum type)
{
	switch (type) {
		case GL_FLOAT: return "float";
		case GL_FLOAT_VEC2: return "vec2";
		case GL_FLOAT_VEC3: return "vec3";
		case GL_FLOAT_VEC4: return "vec4";
		case GL_DOUBLE: return "double";
		case GL_DOUBLE_VEC2: return "dvec2";
		case GL_DOUBLE_VEC3: return "dvec3";
		case GL_DOUBLE_VEC4: return "dvec4";
		case GL_INT: return "int";
		case GL_INT_VEC2: return "ivec2";
		case GL_INT_VEC3: return "ivec3";
		case GL_INT_VEC4: return "ivec4";
		case GL_UNSIGNED_INT: return "unsigned int";
		case GL_UNSIGNED_INT_VEC2: return "uvec2";
		case GL_UNSIGNED_INT_VEC3: return "uvec3";
		case GL_UNSIGNED_INT_VEC4: return "uvec4";
		case GL_BOOL: return "bool";
		case GL_BOOL_VEC2: return "bvec2";
		case GL_BOOL_VEC3: return "bvec3";
		case GL_BOOL_VEC4: return "bvec4";
		case GL_FLOAT_MAT2: return "mat2";
		case GL_FLOAT_MAT3: return "mat3";
		case GL_FLOAT_MAT4: return "mat4";
		case GL_FLOAT_MAT2x3: return "mat2x3";
		case GL_FLOAT_MAT2x4: return "mat2x4";
		case GL_FLOAT_MAT3x2: return "mat3x2";
		case GL_FLOAT_MAT3x4: return "mat3x4";
		case GL_FLOAT_MAT4x2: return "mat4x2";
		case GL_FLOAT_MAT4x3: return "mat4x3";
		case GL_DOUBLE_MAT2: return "dmat2";
		case GL_DOUBLE_MAT3: return "dmat3";
		case GL_DOUBLE_MAT4: return "dmat4";
		case GL_DOUBLE_MAT2x3: return "dmat2x3";
		case GL_DOUBLE_MAT2x4: return "dmat2x4";
		case GL_DOUBLE_MAT3x2: return "dmat3x2";
		case GL_DOUBLE_MAT3x4: return "dmat3x4";
		case GL_DOUBLE_MAT4x2: return "dmat4x2";
		case GL_DOUBLE_MAT4x3: return "dmat4x3";
		case GL_SAMPLER_1D: return "sampler1D";
		case GL_SAMPLER_2D: return "sampler2D";
		case GL_SAMPLER_3D: return "sampler3D";
		case GL_SAMPLER_CUBE: return "samplerCube";
		case GL_SAMPLER_1D_SHADOW: return "sampler1DShadow";
		case GL_SAMPLER_2D_SHADOW: return "sampler2DShadow";
		case GL_SAMPLER_1D_ARRAY: return "sampler1DArray";
		case GL_SAMPLER_2D_ARRAY: return "sampler2DArray";
		case GL_SAMPLER_1D_ARRAY_SHADOW: return "sampler1DArrayShadow";
		case GL_SAMPLER_2D_ARRAY_SHADOW: return "sampler2DArrayShadow";
		case GL_SAMPLER_2D_MULTISAMPLE: return "sampler2DMS";
		case GL_SAMPLER_2D_MULTISAMPLE_ARRAY: return "sampler2DMSArray";
		case GL_SAMPLER_CUBE_SHADOW: return "samplerCubeShadow";
		case GL_SAMPLER_BUFFER: return "samplerBuffer";
		case GL_SAMPLER_2D_RECT: return "sampler2DRect";
		case GL_SAMPLER_2D_RECT_SHADOW: return "sampler2DRectShadow";
		case GL_INT_SAMPLER_1D: return "isampler1D";
		case GL_INT_SAMPLER_2D: return "isampler2D";
		case GL_INT_SAMPLER_3D: return "isampler3D";
		case GL_INT_SAMPLER_CUBE: return "isamplerCube";
		case GL_INT_SAMPLER_1D_ARRAY: return "isampler1DArray";
		case GL_INT_SAMPLER_2D_ARRAY: return "isampler2DArray";
		case GL_INT_SAMPLER_2D_MULTISAMPLE: return "isampler2DMS";
		case GL_INT_SAMPLER_2D_MULTISAMPLE_ARRAY: return "isampler2DMSArray";
		case GL_INT_SAMPLER_BUFFER: return "isamplerBuffer";
		case GL_INT_SAMPLER_2D_RECT: return "isampler2DRect";
		case GL_UNSIGNED_INT_SAMPLER_1D: return "usampler1D";
		case GL_UNSIGNED_INT_SAMPLER_2D: return "usampler2D";
		case GL_UNSIGNED_INT_SAMPLER_3D: return "usampler3D";
		case GL_UNSIGNED_INT_SAMPLER_CUBE: return "usamplerCube";
		case GL_UNSIGNED_INT_SAMPLER_1D_ARRAY: return "usampler1DArray";
		case GL_UNSIGNED_INT_SAMPLER_2D_ARRAY: return "usampler2DArray";
		case GL_UNSIGNED_INT_SAMPLER_2D_MULTISAMPLE: return "usampler2DMS";
		case GL_UNSIGNED_INT_SAMPLER_2D_MULTISAMPLE_ARRAY: return "usampler2DMSArray";
		case GL_UNSIGNED_INT_SAMPLER_BUFFER: return "usamplerBuffer";
		case GL_UNSIGNED_INT_SAMPLER_2D_RECT: return "usampler2DRect";
	}
}
#endif
