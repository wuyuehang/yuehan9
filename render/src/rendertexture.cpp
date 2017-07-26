#include <assert.h>
#include "glrunner.h"

#define _draw_depth_buffer_only 1

GLfloat quad_pos[] = {
		-1.0, 1.0,
		1.0, -1.0,
		-1.0, -1.0,
};

GLfloat quad_uv[] = {
		0.0, 1.0,
		1.0, 0.0,
		0.0, 0.0
};

GLuint VS[2], FS[2];
GLuint pipe;
GLuint VAO[2];

void RenderCB(GlRunner *runner)
{
	glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
	glClearDepth(1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glDrawArrays(GL_TRIANGLES, 0, 3);
}

int main(int argc, char **argv)
{
	GlRunner *runner = new GlRunner(RenderCB);

	VS[0] = runner->BuildShaderProgram("shaders/simple.vert", GL_VERTEX_SHADER);
	FS[0] = runner->BuildShaderProgram("shaders/simple.frag", GL_FRAGMENT_SHADER);
	VS[1] = runner->BuildShaderProgram("shaders/quad.vert", GL_VERTEX_SHADER);
	FS[1] = runner->BuildShaderProgram("shaders/quad.frag", GL_FRAGMENT_SHADER);
	pipe = runner->BuildProgramPipeline();

	glProgramUniform4f(FS[0], glGetUniformLocation(FS[0], "uColor"), 1.0, 1.0, 0.0, 1.0);

	// first pass, render texture
	GLfloat pos_buf[] = {
			-0.5, 0.5, 0.5,
			0.5, 0.5, 0.5,
			0.0, -0.5, 0.5,
	};

	glGenVertexArrays(2, VAO);
	glBindVertexArray(VAO[0]);

	GLuint VBO[3];
	glGenBuffers(3, VBO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO[0]);
	glBufferData(GL_ARRAY_BUFFER, sizeof(pos_buf), pos_buf, GL_STATIC_DRAW);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), (GLvoid*)0);
	glEnableVertexAttribArray(0);

	// first pass
#if !_draw_depth_buffer_only
	GLuint colorb;
	glGenTextures(1, &colorb);
	glBindTexture(GL_TEXTURE_2D, colorb);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, GR_WIDTH, GR_HEIGHT, 0, GL_RGBA, GL_UNSIGNED_BYTE, nullptr);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
#endif

	GLuint depthb;
	glGenTextures(1, &depthb);
	glBindTexture(GL_TEXTURE_2D, depthb);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, GR_WIDTH, GR_HEIGHT, 0, GL_DEPTH_COMPONENT, GL_UNSIGNED_BYTE, nullptr);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	GLuint fbo;
	glGenFramebuffers(1, &fbo);
	glBindFramebuffer(GL_FRAMEBUFFER, fbo);

#if !_draw_depth_buffer_only
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, colorb, 0);
	assert(GL_FRAMEBUFFER_COMPLETE == glCheckFramebufferStatus(GL_FRAMEBUFFER));
#endif
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, depthb, 0);
	assert(GL_FRAMEBUFFER_COMPLETE == glCheckFramebufferStatus(GL_FRAMEBUFFER));

	glBindFramebuffer(GL_DRAW_FRAMEBUFFER, fbo);

#if _draw_depth_buffer_only
	glDrawBuffers(0, GL_NONE);
#endif

#if _draw_depth_buffer_only
	glClearDepthf(1.0);
	glClear(GL_DEPTH_BUFFER_BIT);
#else
	glClearColor(0.35f, 0.25f, 0.45f, 0.5f);
	glClearDepthf(1.0);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
#endif

	glUseProgramStages(pipe, GL_VERTEX_SHADER_BIT, VS[0]);
	glUseProgramStages(pipe, GL_FRAGMENT_SHADER_BIT, FS[0]);
	glDrawArrays(GL_TRIANGLES, 0, 3);

	// second pass
	glBindVertexArray(VAO[1]);
	glBindBuffer(GL_ARRAY_BUFFER, VBO[1]);
	glBufferData(GL_ARRAY_BUFFER, sizeof(quad_pos), quad_pos, GL_STATIC_DRAW);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(GLfloat), (GLvoid *)0);

	glBindBuffer(GL_ARRAY_BUFFER, VBO[2]);
	glBufferData(GL_ARRAY_BUFFER, sizeof(quad_uv), quad_uv, GL_STATIC_DRAW);
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(GLfloat), (GLvoid *)0);

	glActiveTexture(GL_TEXTURE0);

#if _draw_depth_buffer_only
	glBindTexture(GL_TEXTURE_2D, depthb);
#else
	glBindTexture(GL_TEXTURE_2D, colorb);
#endif

	glProgramUniform1i(FS[1], glGetUniformLocation(FS[1], "color2D"), 0);

	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	glUseProgramStages(pipe, GL_VERTEX_SHADER_BIT, VS[1]);
	glUseProgramStages(pipe, GL_FRAGMENT_SHADER_BIT, FS[1]);

	runner->OnRender();

	return 0;
}
