#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "utility.h"
#include "warp_types.h"

#define _draw_depth_texture_only_ 1
struct warp_runtime *wrt = NULL;

int main(int argc, char **argv)
{
	wrt = (struct warp_runtime *)malloc(sizeof(struct warp_runtime));
	memset(wrt, 0, sizeof(struct warp_runtime));

	create_warp_runtime(wrt);

	create_warp_shaders(&wrt->vertex_shader[0], "shaders/simple.vert",
			&wrt->fragment_shader[0], "shaders/constant.frag");

	create_warp_program(&wrt->vertex_shader[0],
			&wrt->fragment_shader[0],
			&wrt->program[0]);

	GLfloat pos_buf[] = {
			-0.5, -0.5,
			 0.5, -0.5,
			 0.0,  0.5
	};

	// 1. render the triangle into a texture
	glBindVertexArray(wrt->vao[0]);

	glBindBuffer(GL_ARRAY_BUFFER, wrt->vbo[0]);
	glBufferData(GL_ARRAY_BUFFER, sizeof(pos_buf), pos_buf, GL_STATIC_DRAW);

	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(GLfloat), (GLvoid*)0);

#if !_draw_depth_texture_only_
	GLuint colorb;
	glGenTextures(1, &colorb);
	glBindTexture(GL_TEXTURE_2D, colorb);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8,
			ONSCREEN_SURFACE_WIDTH, ONSCREEN_SURFACE_HEIGHT, 0,
			GL_RGBA, GL_UNSIGNED_BYTE, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
#endif

	GLuint depthb;
	glGenTextures(1, &depthb);
	glBindTexture(GL_TEXTURE_2D, depthb);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT24,
			ONSCREEN_SURFACE_WIDTH, ONSCREEN_SURFACE_WIDTH, 0,
			GL_DEPTH_COMPONENT, GL_UNSIGNED_INT, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	GLuint fbo;
	glGenFramebuffers(1, &fbo);
	glBindFramebuffer(GL_FRAMEBUFFER, fbo);

#if !_draw_depth_texture_only_
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0,
			GL_TEXTURE_2D, colorb, 0);
	assert(GL_FRAMEBUFFER_COMPLETE == glCheckFramebufferStatus(GL_FRAMEBUFFER));
#endif

	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT,
			GL_TEXTURE_2D, depthb, 0);
	assert(GL_FRAMEBUFFER_COMPLETE == glCheckFramebufferStatus(GL_FRAMEBUFFER));

	glBindFramebuffer(GL_DRAW_FRAMEBUFFER, fbo);

#if _draw_depth_texture_only_
	glDrawBuffers(0, GL_NONE);
	glClearDepthf(1.0);
	glClear(GL_DEPTH_BUFFER_BIT);
#else
	glClearColor(0.35f, 0.25f, 0.45f, 0.5f);
	glClearDepthf(1.0);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
#endif
	glDrawArrays(GL_TRIANGLES, 0, 3);

	// 2. sample the texture into quad
	GLfloat quad_pos_buf[] = {
		-1.f, 1.f,
		-1.f, -1.f,
		1.f, -1.f,
	};

	GLfloat quad_uv_buf[] = {
		0.0, 1.0,
		0.0, 0.0,
		1.0, 0.0,
	};

	glBindVertexArray(wrt->vao[1]);
	glBindBuffer(GL_ARRAY_BUFFER, wrt->vbo[1]);
	glBufferData(GL_ARRAY_BUFFER, sizeof(quad_pos_buf), quad_pos_buf, GL_STATIC_DRAW);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(GLfloat), (GLvoid *)0);

	glBindBuffer(GL_ARRAY_BUFFER, wrt->vbo[2]);
	glBufferData(GL_ARRAY_BUFFER, sizeof(quad_uv_buf), quad_uv_buf, GL_STATIC_DRAW);
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(GLfloat), (GLvoid *)0);

	create_warp_shaders(&wrt->vertex_shader[1], "shaders/quad.vert",
			&wrt->fragment_shader[1], "shaders/quad.frag");

	create_warp_program(&wrt->vertex_shader[1],
			&wrt->fragment_shader[1],
			&wrt->program[1]);

	glActiveTexture(GL_TEXTURE0);
#if _draw_depth_texture_only_
	glBindTexture(GL_TEXTURE_2D, depthb);
#else
	glBindTexture(GL_TEXTURE_2D, colorb);
#endif
	assert(glGetUniformLocation(wrt->program[1], "color2D") != -1);
	glUniform1i(glGetUniformLocation(wrt->program[1], "color2D"), 0);

	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	while (1) {

		XNextEvent(wrt->x11_display, &(wrt->event));

		switch(wrt->event.type) {
		case Expose:
			glClearColor(0.0, 0.0, 0.0, 1.0);
			glClearDepthf(1.0);
			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
			glDrawArrays(GL_TRIANGLES, 0, 3);

			eglSwapBuffers(wrt->egl_display, wrt->egl_surface);
			break;
		case ButtonPress:
		case KeyPress:
			goto finish;
		default:
			break;
		}
	}

finish:
	destroy_warp_runtime(wrt);
	return 0;
}
