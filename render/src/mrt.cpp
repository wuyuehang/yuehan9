#include <glm/glm.hpp>
#include "glrunner.h"

#define QUALD 64
#define QUALD_SQRT 8

GLuint VAO[3];
GLuint VBO[5];
GLuint FS[2], VS[2];
GLuint mrt0, mrt1;

GLfloat pos[] = {
		-0.04f, 0.04f,
		0.04f, 0.04f,
		0.04f, -0.04f,
		-0.04f, 0.04f,
		0.04f, -0.04f,
		-0.04f, -0.04f,
};

GLfloat left_quad[] = {
		-1.0f, 0.5f,
		0.0f, -0.5f,
		0.0f, 0.5f,
		-1.0f, -0.5f,
};

GLfloat right_quad[] = {
		0.0f, 0.5f,
		1.0f, -0.5f,
		1.0f, 0.5f,
		0.0f, -0.5f,
};

GLfloat texcord_quad[] = {
		0.0f, 0.0f,
		1.0f, 1.0f,
		1.0f, 0.0f,
		0.0f, 1.0f,
};

void RenderCB(GlRunner *runner)
{
	glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
	glClearDepth(1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	/* show normal color on left image */
	glBindVertexArray(VAO[1]);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, mrt0);
	glProgramUniform1i(FS[1], glGetUniformLocation(FS[1], "color2D"), 0);
	glDrawArrays(GL_TRIANGLES, 0, 3);

	/* show position attribute on right image */
	glBindVertexArray(VAO[2]);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, mrt1);
	glProgramUniform1i(FS[1], glGetUniformLocation(FS[1], "color2D"), 0);
		glDrawArrays(GL_TRIANGLES, 0, 3);
}

int main(int argc, char **argv)
{
	GlRunner *runner = new GlRunner(RenderCB);

	glGenVertexArrays(3, VAO);
	glGenBuffers(5, VBO);

	/* render to multiple texture */
	glBindVertexArray(VAO[0]);

	glBindBuffer(GL_ARRAY_BUFFER, VBO[0]);
	glBufferData(GL_ARRAY_BUFFER, sizeof(pos), pos, GL_STATIC_DRAW);

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

	glBindBuffer(GL_ARRAY_BUFFER, VBO[1]);
	glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec2) * QUALD, &go[0], GL_STATIC_DRAW);

	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(GLfloat), (GLvoid *)0);
	glVertexAttribDivisor(1, 1);

	VS[0] = runner->BuildShaderProgram("shaders/mrt.vert", GL_VERTEX_SHADER);
	FS[0] = runner->BuildShaderProgram("shaders/mrt.frag", GL_FRAGMENT_SHADER);

	GLuint pipe = runner->BuildProgramPipeline();

	glUseProgramStages(pipe, GL_VERTEX_SHADER_BIT, VS[0]);
	glUseProgramStages(pipe, GL_FRAGMENT_SHADER_BIT, FS[0]);

	/* create off-screen texture */
	glGenTextures(1, &mrt0);
	glBindTexture(GL_TEXTURE_2D, mrt0);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, GR_WIDTH, GR_HEIGHT, 0, GL_RGBA, GL_UNSIGNED_BYTE, nullptr);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	glGenTextures(1, &mrt1);
	glBindTexture(GL_TEXTURE_2D, mrt1);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, GR_WIDTH, GR_HEIGHT, 0, GL_RGBA, GL_UNSIGNED_BYTE, nullptr);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	GLuint fbo;
	glGenFramebuffers(1, &fbo);
	glBindFramebuffer(GL_FRAMEBUFFER, fbo);

	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, mrt0, 0);
	assert(GL_FRAMEBUFFER_COMPLETE == glCheckFramebufferStatus(GL_FRAMEBUFFER));
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT1, GL_TEXTURE_2D, mrt1, 0);
	assert(GL_FRAMEBUFFER_COMPLETE == glCheckFramebufferStatus(GL_FRAMEBUFFER));

	/* tell GPU we need draw mrt */
	GLenum buffers[] = { GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1 };
	glDrawBuffers(2, buffers);

	/* render off-screen buffer */
	glBindFramebuffer(GL_DRAW_FRAMEBUFFER, fbo);

	glClearColor(0.35, 0.25, 0.45, 0.5);
	glClearDepthf(1.0);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glDrawArraysInstanced(GL_TRIANGLES, 0, 6, QUALD);

	/* for left image */
	glBindVertexArray(VAO[1]);

	glBindBuffer(GL_ARRAY_BUFFER, VBO[2]);
	glBufferData(GL_ARRAY_BUFFER, sizeof(left_quad), left_quad, GL_STATIC_DRAW);

	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(GLfloat), (GLvoid *)0);

	glBindBuffer(GL_ARRAY_BUFFER, VBO[3]);
	glBufferData(GL_ARRAY_BUFFER, sizeof(texcord_quad), texcord_quad, GL_STATIC_DRAW);

	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(GLfloat), (GLvoid *)0);

	/* for right image */
	glBindVertexArray(VAO[2]);

	glBindBuffer(GL_ARRAY_BUFFER, VBO[4]);
	glBufferData(GL_ARRAY_BUFFER, sizeof(right_quad), right_quad, GL_STATIC_DRAW);

	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(GLfloat), (GLvoid *)0);

	glBindBuffer(GL_ARRAY_BUFFER, VBO[3]);

	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(GLfloat), (GLvoid *)0);

	VS[1] = runner->BuildShaderProgram("shaders/quad.vert", GL_VERTEX_SHADER);
	FS[1] = runner->BuildShaderProgram("shaders/quad.frag", GL_FRAGMENT_SHADER);

	glUseProgramStages(pipe, GL_VERTEX_SHADER_BIT, VS[1]);
	glUseProgramStages(pipe, GL_FRAGMENT_SHADER_BIT, FS[1]);

	/* close offscreen render, we're sample the result to onscreen */
	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	runner->OnRender();

	return 0;
}
