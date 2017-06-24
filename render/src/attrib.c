#include "ogl_warp.h"

#define separate_per_vertex_data 1

int main()
{
	struct ogl_warp *ow = (struct ogl_warp *)malloc(sizeof(struct ogl_warp));
	memset(ow, 0, sizeof(*ow));

	create_ogl_warp(ow);

	create_ogl_warp_shaders(&ow->vertex_shaders[0], "shaders/attrib2.vert",
			&ow->fragment_shaders[0], "shaders/attrib2.frag");

	create_ogl_warp_program(ow->vertex_shaders[0],
			ow->fragment_shaders[0], &ow->programs[0]);

#if separate_per_vertex_data
	// separate per vertex data
	GLfloat pos_buf[] = {
		0.0, 0.5,
		-0.5, -0.5,
		0.5, -0.5
	};

	GLfloat color_buf[] = {
		1.0, 1.0, 0.0,
		0.0, 1.0, 1.0,
		1.0, 0.0, 1.0
	};

	enum { attr_pos, attr_color};

	glBindBuffer(GL_ARRAY_BUFFER, ow->vbo[0]);
	glBufferData(GL_ARRAY_BUFFER, sizeof(pos_buf), pos_buf, GL_STATIC_DRAW);

	glBindBuffer(GL_ARRAY_BUFFER, ow->vbo[1]);
	glBufferData(GL_ARRAY_BUFFER, sizeof(color_buf), color_buf, GL_STATIC_DRAW);

	glBindVertexArray(ow->vao[0]);

	glEnableVertexAttribArray(attr_pos);
	glEnableVertexAttribArray(attr_color);

	glVertexAttribFormat(attr_pos, 2, GL_FLOAT, GL_FALSE, 0);
	glVertexAttribFormat(attr_color, 3, GL_FLOAT, GL_FALSE, 0);

	glVertexAttribBinding(attr_pos, 0);
	glVertexAttribBinding(attr_color, 1);

	glBindVertexBuffer(0, ow->vbo[0], 0, 2 * sizeof(GLfloat));
	glBindVertexBuffer(1, ow->vbo[1], 0, 3 * sizeof(GLfloat));

#else
	// interleave per vertex data
	GLfloat interleave[] = {
		0.0, 0.5, 1.0, 0.0, 0.0,
		-0.5, -0.5, 0.0, 1.0, 0.0,
		0.5, -0.5, 0.0, 0.0, 1.0
	};

	enum { attr_pos, attr_color};

	glBindBuffer(GL_ARRAY_BUFFER, ow->vbo[0]);
	glBufferData(GL_ARRAY_BUFFER, sizeof(interleave), interleave, GL_STATIC_DRAW);

	glBindVertexArray(ow->vao[0]);

	glEnableVertexAttribArray(attr_pos);
	glEnableVertexAttribArray(attr_color);

	glVertexAttribFormat(attr_pos, 2, GL_FLOAT, GL_FALSE, 0);
	glVertexAttribFormat(attr_color, 3, GL_FLOAT, GL_FALSE, 2 * sizeof(GLfloat));

	glVertexAttribBinding(attr_pos, 0);
	glVertexAttribBinding(attr_color, 0);

	glBindVertexBuffer(0, ow->vbo[0], 0, 5 * sizeof(GLfloat));
#endif

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
