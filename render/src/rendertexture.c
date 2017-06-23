#include <assert.h>
#include "ogl_warp.h"

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

int main(int argc, char **argv)
{
	struct ogl_warp *ow = (struct ogl_warp *)malloc(sizeof(struct ogl_warp));

	create_ogl_warp(ow);

	create_ogl_warp_shaders(&ow->vertex_shaders[0], "shaders/simple.vert",
			&ow->fragment_shaders[0], "shaders/simple.frag");

	create_ogl_warp_program(ow->vertex_shaders[0],
			ow->fragment_shaders[0],
			&ow->programs[0]);

	glUniform4f(glGetUniformLocation(ow->programs[0], "uColor"), 1.0, 1.0, 0.0, 1.0);

	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LESS);

	// first pass, render texture
	GLfloat pos_buf[] = {
			-0.5, 0.5, 0.5,
			0.5, 0.5, 0.5,
			0.0, -0.5, 0.5,
	};

	glBindVertexArray(ow->vao[0]);
	glBindBuffer(GL_ARRAY_BUFFER, ow->vbo[0]);
	glBufferData(GL_ARRAY_BUFFER, sizeof(pos_buf), pos_buf, GL_STATIC_DRAW);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), (GLvoid*)0);
	glEnableVertexAttribArray(0);

	// first pass
#if !_draw_depth_buffer_only
	GLuint colorb;
	glGenTextures(1, &colorb);
	glBindTexture(GL_TEXTURE_2D, colorb);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, OGL_WIN_WIDTH, OGL_WIN_HEIGHT, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
#endif

	GLuint depthb;
	glGenTextures(1, &depthb);
	glBindTexture(GL_TEXTURE_2D, depthb);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, OGL_WIN_WIDTH, OGL_WIN_HEIGHT, 0, GL_DEPTH_COMPONENT, GL_UNSIGNED_BYTE, NULL);
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

	glDrawArrays(GL_TRIANGLES, 0, 3);

	// second pass
	glBindVertexArray(ow->vao[1]);
	glBindBuffer(GL_ARRAY_BUFFER, ow->vbo[1]);
	glBufferData(GL_ARRAY_BUFFER, sizeof(quad_pos), quad_pos, GL_STATIC_DRAW);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(GLfloat), (GLvoid *)0);

	glBindBuffer(GL_ARRAY_BUFFER, ow->vbo[2]);
	glBufferData(GL_ARRAY_BUFFER, sizeof(quad_uv), quad_uv, GL_STATIC_DRAW);
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(GLfloat), (GLvoid *)0);

	create_ogl_warp_shaders(&ow->vertex_shaders[1], "shaders/quad.vert",
			&ow->fragment_shaders[1], "shaders/quad.frag");

	create_ogl_warp_program(ow->vertex_shaders[1], ow->fragment_shaders[1],
			&ow->programs[1]);

	glActiveTexture(GL_TEXTURE0);

#if _draw_depth_buffer_only
	glBindTexture(GL_TEXTURE_2D, depthb);
#else
	glBindTexture(GL_TEXTURE_2D, colorb);
#endif

	assert(glGetUniformLocation(ow->programs[1], "color2D") != -1);
	glUniform1i(glGetUniformLocation(ow->programs[1], "color2D"), 0);

	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	while (!glfwWindowShouldClose(ow->win))
	{
		glfwPollEvents();
		glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
		glClearDepth(1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		glDrawArrays(GL_TRIANGLES, 0, 3);
		glfwSwapBuffers(ow->win);
	}

	glfwTerminate();

	return 0;
}
