#include "glrunner.h"

GLuint VAO, VBO;

void RenderCB(GlRunner *runner)
{
	glClearColor(0.0, 0.0, 0.0, 1.0);
	glClearDepthf(1.0);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
}

int main()
{
	GlRunner *runner = new GlRunner(RenderCB);

	GLuint vs = runner->BuildShaderProgram("shaders/simple.vert", GL_VERTEX_SHADER);
	GLuint fs = runner->BuildShaderProgram("shaders/fragcord.frag", GL_FRAGMENT_SHADER);

	GLuint pipeline = runner->BuildProgramPipeline();

	glUseProgramStages(pipeline, GL_VERTEX_SHADER_BIT, vs);
	glUseProgramStages(pipeline, GL_FRAGMENT_SHADER_BIT, fs);

	glProgramUniform2f(fs, glGetUniformLocation(fs, "resinfo"), (float)GR_WIDTH, (float)GR_HEIGHT);

	GLfloat pos[] = {
		1.0, 1.0,
		-1.0, 1.0,
		1.0, -1.0,
		-1.0, -1.0
	};

	GLuint VBO, VAO;
	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &VBO);

	glBindVertexArray(VAO);

	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(pos), pos, GL_STATIC_DRAW);

	glEnableVertexAttribArray(0);
	glVertexAttribFormat(0, 2, GL_FLOAT, GL_FALSE, 0);
	glVertexAttribBinding(0, 0);
	glBindVertexBuffer(0, VBO, 0, 2*sizeof(GLfloat));

	runner->OnRender();

	return 0;
}
