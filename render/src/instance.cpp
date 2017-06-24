#include <glm/glm.hpp>
#include "ogl_warp.h"

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

int main()
{
	struct ogl_warp *ow = (struct ogl_warp *)malloc(sizeof(struct ogl_warp));
	memset(ow, 0, sizeof(*ow));

	create_ogl_warp(ow);

	create_ogl_warp_shaders(&ow->vertex_shaders[0], "shaders/instance.vert",
			&ow->fragment_shaders[0], "shaders/simple.frag");

	create_ogl_warp_program(ow->vertex_shaders[0],
			ow->fragment_shaders[0], &ow->programs[0]);

	glUniform4f(glGetUniformLocation(ow->programs[0], "uColor"), 1.0, 1.0, 1.0, 1.0);

#if _enable_blend_
	glEnable(GL_BLEND);
	glBlendFunc(GL_DST_ALPHA, GL_ONE_MINUS_DST_ALPHA);
#endif

	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LESS);

	glBindVertexArray(ow->vao[0]);

	// per vertex data
	glBindBuffer(GL_ARRAY_BUFFER, ow->vbo[0]);
	glBufferData(GL_ARRAY_BUFFER, sizeof(pos_buf), pos_buf, GL_STATIC_DRAW);

	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(GLfloat), (GLvoid *)0);

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

	glBindBuffer(GL_ARRAY_BUFFER, ow->vbo[1]);
	glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec2) * QUALD, &go[0], GL_STATIC_DRAW);

	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(GLfloat), (GLvoid *)0);
	glVertexAttribDivisor(1, 1);

	while (!glfwWindowShouldClose(ow->win))
	{
		glfwPollEvents();
		glClearColor(0.05f, 0.05f, 0.05f, 0.50f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		glDrawArraysInstanced(GL_TRIANGLES, 0, 6, QUALD);
		glfwSwapBuffers(ow->win);
	}

	glfwTerminate();

	return 0;
}
