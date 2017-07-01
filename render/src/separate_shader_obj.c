#include "ogl_warp.h"

int main()
{
	struct ogl_warp *ow = (struct ogl_warp *)malloc(sizeof(struct ogl_warp));

	create_ogl_warp(ow);

	GLfloat pos[] = {
		-1.0, 1.0,
		-1.0, -1.0,
		1.0, -1.0
	};

	glBindVertexArray(ow->vao[0]);
	glBindBuffer(GL_ARRAY_BUFFER, ow->vbo[0]);
	glBufferData(GL_ARRAY_BUFFER, sizeof(pos), pos, GL_STATIC_DRAW);

	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(GLfloat), (GLvoid *)0);
	glEnableVertexAttribArray(0);

	char *source;
	load_shader_from_file("shaders/simple.vert", &source);

	GLuint vsProg = glCreateShaderProgramv(GL_VERTEX_SHADER, 1, &source);

	load_shader_from_file("shaders/simple.frag", &source);
	GLuint fsProg = glCreateShaderProgramv(GL_FRAGMENT_SHADER, 1, &source);
	GLuint color_uni = glGetUniformLocation(fsProg, "uColor");

	glProgramUniform4f(fsProg, color_uni, 1.0, 1.0, 0.0, 1.0);

	GLuint ppline;
	glGenProgramPipelines(1, &ppline);
	glBindProgramPipeline(ppline);

	glUseProgramStages(ppline, GL_VERTEX_SHADER_BIT, vsProg);
	glUseProgramStages(ppline, GL_FRAGMENT_SHADER_BIT, fsProg);

	while (!glfwWindowShouldClose(ow->win))
	{
		glfwPollEvents();

		glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
		glClear(GL_COLOR_BUFFER_BIT);

		glDrawArrays(GL_TRIANGLES, 0, 3);
		glfwSwapBuffers(ow->win);
	}

	glfwTerminate();

	return 0;
}
