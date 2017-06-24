#include <glm/glm.hpp>
#include "ogl_warp.h"

#define QUALD 64
#define QUALD_SQRT 8

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

GLchar *source = NULL;

int main(int argc, char **argv)
{
	struct ogl_warp *ow = (struct ogl_warp *)malloc(sizeof(struct ogl_warp));
	memset(ow, 0, sizeof(*ow));

	create_ogl_warp(ow);

	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LESS);

	/* render to multiple texture */
	glBindVertexArray(ow->vao[0]);

	glBindBuffer(GL_ARRAY_BUFFER, ow->vbo[0]);
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

	glBindBuffer(GL_ARRAY_BUFFER, ow->vbo[1]);
	glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec2) * QUALD, &go[0], GL_STATIC_DRAW);

	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(GLfloat), (GLvoid *)0);
	glVertexAttribDivisor(1, 1);

	create_ogl_warp_shaders(&ow->vertex_shaders[0], "shaders/mrt.vert",
			&ow->fragment_shaders[0], "shaders/mrt.frag");

	create_ogl_warp_program(ow->vertex_shaders[0],
			ow->fragment_shaders[0], &ow->programs[0]);

	/* create off-screen texture */
	GLuint mrt0;
	glGenTextures(1, &mrt0);
	glBindTexture(GL_TEXTURE_2D, mrt0);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, OGL_WIN_WIDTH, OGL_WIN_HEIGHT, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	GLuint mrt1;
	glGenTextures(1, &mrt1);
	glBindTexture(GL_TEXTURE_2D, mrt1);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, OGL_WIN_WIDTH, OGL_WIN_HEIGHT, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);
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
	glBindVertexArray(ow->vao[1]);

	glBindBuffer(GL_ARRAY_BUFFER, ow->vbo[2]);
	glBufferData(GL_ARRAY_BUFFER, sizeof(left_quad), left_quad, GL_STATIC_DRAW);

	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(GLfloat), (GLvoid *)0);

	glBindBuffer(GL_ARRAY_BUFFER, ow->vbo[3]);
	glBufferData(GL_ARRAY_BUFFER, sizeof(texcord_quad), texcord_quad, GL_STATIC_DRAW);

	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(GLfloat), (GLvoid *)0);

	/* for right image */
	glBindVertexArray(ow->vao[2]);

	glBindBuffer(GL_ARRAY_BUFFER, ow->vbo[4]);
	glBufferData(GL_ARRAY_BUFFER, sizeof(right_quad), right_quad, GL_STATIC_DRAW);

	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(GLfloat), (GLvoid *)0);

	glBindBuffer(GL_ARRAY_BUFFER, ow->vbo[3]);

	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(GLfloat), (GLvoid *)0);

	create_ogl_warp_shaders(&ow->vertex_shaders[1], "shaders/quad.vert",
			&ow->fragment_shaders[1], "shaders/quad.frag");

	create_ogl_warp_program(ow->vertex_shaders[1],
			ow->fragment_shaders[1], &ow->programs[1]);

	/* close offscreen render, we're sample the result to onscreen */
	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	while (!glfwWindowShouldClose(ow->win))
	{
		glfwPollEvents();

		glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
		glClearDepth(1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		/* show normal color on left image */
		glBindVertexArray(ow->vao[1]);
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, mrt0);
		glUniform1i(glGetUniformLocation(ow->programs[1], "color2D"), 0);
		glDrawArrays(GL_TRIANGLES, 0, 3);

		/* show position attribute on right image */
		glBindVertexArray(ow->vao[2]);
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, mrt1);
		glUniform1i(glGetUniformLocation(ow->programs[1], "color2D"), 0);
		glDrawArrays(GL_TRIANGLES, 0, 3);

		glfwSwapBuffers(ow->win);
	}

	glfwTerminate();

	return 0;
}
