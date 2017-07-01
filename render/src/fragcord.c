#include "ogl_warp.h"

int main()
{
	struct ogl_warp *ow = (struct ogl_warp *)malloc(sizeof(struct ogl_warp));

	create_ogl_warp(ow);

	create_ogl_warp_shaders(&ow->vertex_shaders[0], "shaders/simple.vert",
			&ow->fragment_shaders[0], "shaders/fragcord.frag");

	create_ogl_warp_program(ow->vertex_shaders[0],
			ow->fragment_shaders[0], &ow->programs[0]);

	glUniform2f(glGetUniformLocation(ow->programs[0], "resinfo"), (float)OGL_WIN_WIDTH, (float)OGL_WIN_HEIGHT);

	GLfloat pos[] = {
		1.0, 1.0,
		-1.0, 1.0,
		1.0, -1.0,
		-1.0, -1.0
	};

	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LESS);

	glBindVertexArray(ow->vao[0]);
	glBindBuffer(GL_ARRAY_BUFFER, ow->vbo[0]);
	glBufferData(GL_ARRAY_BUFFER, sizeof(pos), pos, GL_STATIC_DRAW);

	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2*sizeof(GLfloat), (GLvoid *)0);
	glEnableVertexAttribArray(0);

	while (!glfwWindowShouldClose(ow->win)) {
		glfwPollEvents();

		glClearColor(0.0, 0.0, 0.0, 1.0);
		glClearDepthf(1.0);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
		glfwSwapBuffers(ow->win);
	}

	glfwTerminate();

	return 0;
}
