#ifndef _WARP_H_
#define _WARP_H_

#include <iostream>
#include <fstream>
#include <cassert>

void ReadFile(const char *pfile, std::string& source);
GLuint BuildShader(const char *pfile, GLenum shaderType);
GLuint BuildProgram(GLuint VSobj, GLuint FSobj);

void ReadFile(const char *pfile, std::string& source)
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

GLuint BuildShader(const char *pfile, GLenum shaderType)
{
	GLchar message[512];
	GLint status;
	GLuint ShaderObj;
	std::string source;

	assert(pfile);

	ShaderObj = glCreateShader(shaderType);

	ReadFile(pfile, source);

	const GLchar* token = source.c_str();

	glShaderSource(ShaderObj, 1, &token, NULL);

	glCompileShader(ShaderObj);

	glGetShaderiv(ShaderObj, GL_COMPILE_STATUS, &status);

	if (!status) {
		glGetShaderInfoLog(ShaderObj, 512, NULL, message);
		printf("%s\n", message);
	}

	return ShaderObj;
}

GLuint BuildProgram(GLuint VSobj, GLuint FSobj)
{
	GLchar message[512];
	GLint status;
	GLuint ProgramObj;

	ProgramObj = glCreateProgram();

	if (VSobj)
		glAttachShader(ProgramObj, VSobj);

	if (FSobj)
		glAttachShader(ProgramObj, FSobj);

	glLinkProgram(ProgramObj);

	glGetProgramiv(ProgramObj, GL_LINK_STATUS, &status);

	if (!status) {
		glGetProgramInfoLog(ProgramObj, 512, NULL, message);
		printf("%s\n", message);
	}

	glUseProgram(ProgramObj);

	return ProgramObj;
}

#endif
