#include <assert.h>
#include "glrunner.h"

#define _draw_depth_buffer_only 1

GLuint colorb;
GLuint depthb;

void RenderCB(GlRunner *runner)
{
	glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
	glClearDepth(1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

#if _draw_depth_buffer_only
	runner->DrawQuad(depthb);
#else
	runner->DrawQuad(colorb);
#endif
}

int main(int argc, char **argv)
{
	GlRunner *runner = new GlRunner(RenderCB);

	GLuint VS = runner->BuildShaderProgram("shaders/simple.vert", GL_VERTEX_SHADER);
	GLuint FS = runner->BuildShaderProgram("shaders/simple.frag", GL_FRAGMENT_SHADER);
	GLuint pipe = runner->BuildProgramPipeline();

	glProgramUniform4f(FS, glGetUniformLocation(FS, "uColor"), 1.0, 1.0, 0.0, 1.0);

	// first pass, render texture
	GLfloat pos_buf[] = {
			-0.5, 0.5, 0.5,
			0.5, 0.5, 0.5,
			0.0, -0.5, 0.5,
	};

	GLuint VAO;
	glGenVertexArrays(1, &VAO);
	glBindVertexArray(VAO);

	GLuint VBO;
	glGenBuffers(1, &VBO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(pos_buf), pos_buf, GL_STATIC_DRAW);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), (GLvoid*)0);
	glEnableVertexAttribArray(0);

	// first pass
#if !_draw_depth_buffer_only
	glGenTextures(1, &colorb);
	glBindTexture(GL_TEXTURE_2D, colorb);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, GR_WIDTH, GR_HEIGHT, 0, GL_RGBA, GL_UNSIGNED_BYTE, nullptr);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
#endif

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

	glBindProgramPipeline(pipe);
	glUseProgramStages(pipe, GL_VERTEX_SHADER_BIT, VS);
	glUseProgramStages(pipe, GL_FRAGMENT_SHADER_BIT, FS);
	glDrawArrays(GL_TRIANGLES, 0, 3);

	// bind back to onscreen fb0, we're drawing a quad onto
	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	runner->OnRender();

	return 0;
}
