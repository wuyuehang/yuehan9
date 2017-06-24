#include <glm/glm.hpp>
#include "ogl_warp.h"

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

int main(int argc, char **argv)
{
	struct ogl_warp *ow = (struct ogl_warp *)malloc(sizeof(struct ogl_warp));
	memset(ow, 0, sizeof(*ow));

	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LESS);

	create_ogl_warp(ow);

	create_ogl_warp_shaders(&ow->vertex_shaders[0], "shaders/instance.vert",
			&ow->fragment_shaders[0], "shaders/simple.frag");

	create_ogl_warp_program(ow->vertex_shaders[0],
			ow->fragment_shaders[0], &ow->programs[0]);

	glUniform4f(glGetUniformLocation(ow->programs[0], "uColor"), 1.0, 1.0, 1.0, 1.0);
	// preamble resource
	glBindVertexArray(ow->vao[0]);

	glBindBuffer(GL_ARRAY_BUFFER, ow->vbo[0]);
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

	glBindBuffer(GL_ARRAY_BUFFER, ow->vbo[1]);
	glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec2) * QUALD, &go[0], GL_STATIC_DRAW);

	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(GLfloat), (GLvoid *)0);
	glVertexAttribDivisor(1, 1);

	/* create off-screen render buffer */
	GLuint rbo_color;
	glGenRenderbuffers(1, &rbo_color);
	glBindRenderbuffer(GL_RENDERBUFFER, rbo_color);
	glRenderbufferStorage(GL_RENDERBUFFER, GL_RGBA, OGL_WIN_WIDTH, OGL_WIN_HEIGHT);

	GLuint rbo_depth;
	glGenRenderbuffers(1, &rbo_depth);
	glBindRenderbuffer(GL_RENDERBUFFER, rbo_depth);
	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT24, OGL_WIN_WIDTH, OGL_WIN_HEIGHT);

	GLuint fbo;
	glGenFramebuffers(1, &fbo);
	glBindFramebuffer(GL_DRAW_FRAMEBUFFER, fbo);

	glFramebufferRenderbuffer(GL_DRAW_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_RENDERBUFFER, rbo_color);
	assert(GL_FRAMEBUFFER_COMPLETE == glCheckFramebufferStatus(GL_DRAW_FRAMEBUFFER));
	glFramebufferRenderbuffer(GL_DRAW_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, rbo_depth);
	assert(GL_FRAMEBUFFER_COMPLETE == glCheckFramebufferStatus(GL_DRAW_FRAMEBUFFER));

	/* render off-screen buffer */
	glBindFramebuffer(GL_DRAW_FRAMEBUFFER, fbo);

	glClearColor(0.35, 0.25, 0.45, 0.5);
	glClearDepthf(1.0);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glDrawArraysInstanced(GL_TRIANGLES, 0, 6, QUALD);

	while (!glfwWindowShouldClose(ow->win))
	{
		glfwPollEvents();

		glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);
		glBindFramebuffer(GL_READ_FRAMEBUFFER, fbo);

		glBlitFramebuffer(0, 0, OGL_WIN_WIDTH, OGL_WIN_HEIGHT, 0, 0, OGL_WIN_WIDTH, OGL_WIN_HEIGHT, GL_COLOR_BUFFER_BIT, GL_NEAREST);
		glfwSwapBuffers(ow->win);
	}

	glfwTerminate();

	return 0;
}
