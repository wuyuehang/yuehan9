#include "ogl_warp.h"

int main()
{
	struct ogl_warp *ow = (struct ogl_warp *)malloc(sizeof(struct ogl_warp));

	create_ogl_warp(ow);

	create_ogl_warp_shaders(&ow->vertex_shaders[0], "shaders/simple.vert",
			&ow->fragment_shaders[0], "shaders/simple.frag");

	create_ogl_warp_program(ow->vertex_shaders[0],
			ow->fragment_shaders[0], &ow->programs[0]);

	GLfloat rp1_pos[] = {
		0.0, 0.5,
		-0.5, -0.5,
		0.5, -0.5
	};

	GLfloat rp2_pos[] = {
		0.0, 1.0,
		-1.0, -1.0,
		1.0, -1.0
	};

	// bind the smaller triangle for generate stencil buffer to 1'
	glBindVertexArray(ow->vao[0]);
	glBindBuffer(GL_ARRAY_BUFFER, ow->vbo[0]);
	glBufferData(GL_ARRAY_BUFFER, sizeof(rp1_pos), rp1_pos, GL_STATIC_DRAW);
	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2*sizeof(GLfloat), (GLvoid *)0);
	glEnableVertexAttribArray(0);

	// bind the larger triangle for compare stencil buffer
	glBindVertexArray(ow->vao[1]);
	glBindBuffer(GL_ARRAY_BUFFER, ow->vbo[1]);
	glBufferData(GL_ARRAY_BUFFER, sizeof(rp2_pos), rp2_pos, GL_STATIC_DRAW);
	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2*sizeof(GLfloat), (GLvoid *)0);
	glEnableVertexAttribArray(0);

	glEnable(GL_STENCIL_TEST);

	while (!glfwWindowShouldClose(ow->win)) {
		glfwPollEvents();

		glClearColor(0.0, 0.0, 0.0, 1.0);
		glClearStencil(0);
		glClear(GL_COLOR_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

		// smaller triangle should always pass stencil test
		// with depth test, we replace the stencil buffer with
		// reference value
		glStencilFunc(GL_ALWAYS, 1, 0xff);
		glStencilOp(GL_KEEP, GL_KEEP, GL_REPLACE);
		glStencilMask(0xff);

		glBindVertexArray(ow->vao[0]);
		glUniform4f(glGetUniformLocation(ow->programs[0], "uColor"), 1.0, 0.0, 0.0, 0.0);
		glDrawArrays(GL_TRIANGLES, 0, 3);

		// larger triangle compare the stencil (1'), if failed (border)
		// we paint,
		glStencilFunc(GL_NOTEQUAL, 1, 0xff);
		//glStencilOp(GL_ZERO, GL_KEEP, GL_KEEP);
		glStencilMask(0x0);

		glBindVertexArray(ow->vao[1]);
		glUniform4f(glGetUniformLocation(ow->programs[0], "uColor"), 1.0, 1.0, 0.0, 0.0);
		glDrawArrays(GL_TRIANGLES, 0, 3);

		glfwSwapBuffers(ow->win);
	}
	glfwTerminate();

	return 0;
}
