#include <glm/glm.hpp>
#include "glrunner.h"

#define QUALD 64
#define QUALD_SQRT 8

GLfloat pos_buf[] = {
		-0.04f, 0.04f,
		0.04f, 0.04f,
		0.04f, -0.04f,
		-0.04f, 0.04f,
		0.04f, -0.04f,
		-0.04f, -0.04f,
};

GLuint VS, FS, FBO;

void RenderCB(GlRunner *runner)
{
	// clear should be operated on default framebuffer 0
	glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);

	glClearColor(0.35, 0.25, 0.45, 0.5);
	glClearDepthf(1.0);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glBindFramebuffer(GL_READ_FRAMEBUFFER, FBO);

	glBlitFramebuffer(0, 0, GR_WIDTH, GR_HEIGHT, 0, 0, GR_WIDTH, GR_HEIGHT, GL_COLOR_BUFFER_BIT, GL_NEAREST);
}

int main(int argc, char **argv)
{
	GlRunner *runner = new GlRunner(RenderCB);
	VS = runner->BuildShaderProgram("shaders/instance.vert", GL_VERTEX_SHADER);
	FS = runner->BuildShaderProgram("shaders/simple.vert", GL_FRAGMENT_SHADER);

	GLuint pipe = runner->BuildProgramPipeline();

	glUseProgramStages(pipe, GL_VERTEX_SHADER_BIT, VS);
	glUseProgramStages(pipe, GL_FRAGMENT_SHADER_BIT, FS);

	glProgramUniform4f(FS, glGetUniformLocation(FS, "uColor"), 1.0, 1.0, 1.0, 1.0);

	GLuint VAO;
	// preamble resource
	glGenVertexArrays(1, &VAO);
	glBindVertexArray(VAO);

	GLuint VBO[2];
	glGenBuffers(2, VBO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO[0]);
	glBufferData(GL_ARRAY_BUFFER, sizeof(pos_buf), pos_buf, GL_STATIC_DRAW);

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

	/* create off-screen render buffer */
	GLuint rbo_color;
	glGenRenderbuffers(1, &rbo_color);
	glBindRenderbuffer(GL_RENDERBUFFER, rbo_color);
	glRenderbufferStorage(GL_RENDERBUFFER, GL_RGBA, GR_WIDTH, GR_HEIGHT);

	GLuint rbo_depth;
	glGenRenderbuffers(1, &rbo_depth);
	glBindRenderbuffer(GL_RENDERBUFFER, rbo_depth);
	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT24, GR_WIDTH, GR_HEIGHT);

	glGenFramebuffers(1, &FBO);
	glBindFramebuffer(GL_DRAW_FRAMEBUFFER, FBO);

	glFramebufferRenderbuffer(GL_DRAW_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_RENDERBUFFER, rbo_color);
	assert(GL_FRAMEBUFFER_COMPLETE == glCheckFramebufferStatus(GL_DRAW_FRAMEBUFFER));
	glFramebufferRenderbuffer(GL_DRAW_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, rbo_depth);
	assert(GL_FRAMEBUFFER_COMPLETE == glCheckFramebufferStatus(GL_DRAW_FRAMEBUFFER));

	/* render off-screen buffer */
	glBindFramebuffer(GL_DRAW_FRAMEBUFFER, FBO);

	glClearColor(0.35, 0.25, 0.45, 0.5);
	glClearDepthf(1.0);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glDrawArraysInstanced(GL_TRIANGLES, 0, 6, QUALD);

	runner->OnRender();

	return 0;
}
