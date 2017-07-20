#include "glrunner.h"

void RenderCB(GlRunner *runner)
{
	glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
	glClearDepth(1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glDrawArrays(GL_TRIANGLES, 0, 3);
}

int main()
{
	GlRunner *runner = new GlRunner(RenderCB);

	GLfloat pos[] = {
		-1.0, 1.0,
		-1.0, -1.0,
		1.0, -1.0
	};

	GLuint vao, vbo;
	glGenVertexArrays(1, &vao);
	glGenBuffers(1, &vbo);

	glBindVertexArray(vao);
	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	glBufferData(GL_ARRAY_BUFFER, sizeof(pos), pos, GL_STATIC_DRAW);

	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(GLfloat), (GLvoid *)0);
	glEnableVertexAttribArray(0);

	GLuint vsProgram = runner->BuildShaderProgram("shaders/simple.vert", GL_VERTEX_SHADER);

	GLuint fsProgram = runner->BuildShaderProgram("shaders/simple.frag", GL_FRAGMENT_SHADER);

	glProgramUniform4f(fsProgram, glGetUniformLocation(fsProgram, "uColor"), 1.0, 1.0, 0.0, 1.0);

	GLuint ppline = runner->BuildProgramPipeline();

	glUseProgramStages(ppline, GL_VERTEX_SHADER_BIT, vsProgram);
	glUseProgramStages(ppline, GL_FRAGMENT_SHADER_BIT, fsProgram);

	runner->OnRender();

	return 0;
}
