#include "glrunner.h"

GLuint VAO[2];
GLuint VBO[2];
GLuint VS, FS;

void RenderCB(GlRunner *runner)
{
	glClearColor(0.0, 0.0, 0.0, 1.0);
	glClearDepthf(1.0);
	glClearStencil(0);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

	// smaller triangle should always pass stencil test
	// with depth test, we replace the stencil buffer with
	// reference value
	glStencilFunc(GL_ALWAYS, 1, 0xff);
	glStencilOp(GL_KEEP, GL_KEEP, GL_REPLACE);
	glStencilMask(0xff);

	glBindVertexArray(VAO[0]);
	glProgramUniform4f(FS, glGetUniformLocation(FS, "uColor"), 1.0, 0.0, 0.0, 0.0);
	glDrawArrays(GL_TRIANGLES, 0, 3);

	// larger triangle compare the stencil (1'), if failed (border)
	// we paint,
	glStencilFunc(GL_NOTEQUAL, 1, 0xff);
	//glStencilOp(GL_ZERO, GL_KEEP, GL_KEEP);
	glStencilMask(0x0);

	glBindVertexArray(VAO[1]);
	glProgramUniform4f(FS, glGetUniformLocation(FS, "uColor"), 1.0, 1.0, 0.0, 0.0);
	glDrawArrays(GL_TRIANGLES, 0, 3);
}

int main()
{
	GlRunner *runner = new GlRunner(RenderCB);

	VS = runner->BuildShaderProgram("shaders/simple.vert", GL_VERTEX_SHADER);
	FS = runner->BuildShaderProgram("shaders/simple.frag", GL_FRAGMENT_SHADER);

	GLuint pipe = runner->BuildProgramPipeline();

	glUseProgramStages(pipe, GL_VERTEX_SHADER_BIT, VS);
	glUseProgramStages(pipe, GL_FRAGMENT_SHADER_BIT, FS);

	GLfloat rp1_pos[] = {
		0.0, 0.5,
		-0.5, -0.5,
		0.5, -0.5
	};

	GLfloat rp2_pos[] = {
		0.0, 1.0,
		-1.0, -1.0,
		1.0, -1.0
	};

	glGenVertexArrays(2, VAO);
	glGenBuffers(2, VBO);

	// bind the smaller triangle for generate stencil buffer to 1'
	glBindVertexArray(VAO[0]);
	glBindBuffer(GL_ARRAY_BUFFER, VBO[0]);
	glBufferData(GL_ARRAY_BUFFER, sizeof(rp1_pos), rp1_pos, GL_STATIC_DRAW);
	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2*sizeof(GLfloat), (GLvoid *)0);
	glEnableVertexAttribArray(0);

	// bind the larger triangle for compare stencil buffer
	glBindVertexArray(VAO[1]);
	glBindBuffer(GL_ARRAY_BUFFER, VBO[1]);
	glBufferData(GL_ARRAY_BUFFER, sizeof(rp2_pos), rp2_pos, GL_STATIC_DRAW);
	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2*sizeof(GLfloat), (GLvoid *)0);
	glEnableVertexAttribArray(0);

	glEnable(GL_STENCIL_TEST);

	runner->OnRender();

	return 0;
}
