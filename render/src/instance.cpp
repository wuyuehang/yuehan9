#include <glm/glm.hpp>
#include "glrunner.h"

#define QUALD 64
#define QUALD_SQRT 8

#define _enable_blend_ 1

GLfloat pos_buf[] = {
		-0.02f, 0.02f,
		0.02f, 0.02f,
		0.02f, -0.02f,
		-0.02f, 0.02f,
		0.02f, -0.02f,
		-0.02f, -0.02f,
};

void RenderCB(GlRunner *runner)
{
	glClearColor(0.05f, 0.05f, 0.05f, 0.50f);
	glClearDepthf(1.0);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glDrawArraysInstanced(GL_TRIANGLES, 0, 6, QUALD);
}

int main()
{
	GlRunner *runner = new GlRunner(RenderCB);

	GLuint VS = runner->BuildShaderProgram("shaders/instance.vert", GL_VERTEX_SHADER);
	GLuint FS = runner->BuildShaderProgram("shaders/simple.frag", GL_FRAGMENT_SHADER);
	GLuint PIPE = runner->BuildProgramPipeline();

	glUseProgramStages(PIPE, GL_VERTEX_SHADER_BIT, VS);
	glUseProgramStages(PIPE, GL_FRAGMENT_SHADER_BIT, FS);

	glProgramUniform4f(FS, glGetUniformLocation(FS, "uColor"), 1.0, 1.0, 1.0, 1.0);

#if _enable_blend_
	glEnable(GL_BLEND);
	glBlendFunc(GL_DST_ALPHA, GL_ONE_MINUS_DST_ALPHA);
#endif

	GLuint VAO;
	GLuint VBO[2];
	glGenVertexArrays(1, &VAO);
	glBindVertexArray(VAO);

	// per vertex data
	glGenBuffers(2, VBO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO[0]);
	glBufferData(GL_ARRAY_BUFFER, sizeof(pos_buf), pos_buf, GL_STATIC_DRAW);

	glEnableVertexAttribArray(0);
	glVertexAttribFormat(0, 2, GL_FLOAT, GL_FALSE, 0);
	glVertexAttribBinding(0, 0);
	glBindVertexBuffer(0, VBO[0], 0, 2*sizeof(GLfloat));

	// per instance data
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

	glBindBuffer(GL_ARRAY_BUFFER, VBO[1]);
	glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec2) * QUALD, &go[0], GL_STATIC_DRAW);

	glEnableVertexAttribArray(1);
	glVertexAttribFormat(1, 2, GL_FLOAT, GL_FALSE, 0);
	glVertexAttribBinding(1, 1);
	glBindVertexBuffer(1, VBO[1], 0, 2*sizeof(GLfloat));
	glVertexAttribDivisor(1, 1);

	runner->OnRender();

	return 0;
}
