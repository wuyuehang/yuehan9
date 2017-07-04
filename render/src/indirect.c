#include "ogl_warp.h"

#define _enable_multi_draw_indirect_ 1

int main()
{
	struct ogl_warp *ow = (struct ogl_warp *)malloc(sizeof(struct ogl_warp));

	create_ogl_warp(ow);

	create_ogl_warp_shaders(&ow->vertex_shaders[0], "shaders/simple.vert",
			&ow->fragment_shaders[0], "shaders/simple.frag");

	create_ogl_warp_program(ow->vertex_shaders[0],
			ow->fragment_shaders[0], &ow->programs[0]);

	GLfloat pos[] = {
		-0.3, 0.7,
		0.3, 0.7,
		0.7, 0.3,
		0.7, -0.3,
		0.3, -0.7,
		-0.3, -0.7,
		-0.7, -0.3,
		-0.7, 0.3
	};

	typedef struct {
		GLuint vertex_count;
		GLuint instance_count;
		GLuint first_vertex;
		GLuint base_instance;
	} DrawArraysIndirectCommand;

	DrawArraysIndirectCommand draw_indirect_cmd[] = {
		{
			3, // vertex count
			1, // instance count
			0, // first vertex
			0	// base instance
		},
		{
			3,
			1,
			2,
			0
		}
	};

	typedef struct {
		GLuint vertex_count;
		GLuint instance_count;
		GLuint first_index;
		GLuint first_vertex;
		GLuint base_instace;
	} DrawElementsIndirectCommand;

	DrawElementsIndirectCommand draw_element_indirect_cmd[] = {
		{
			3,
			1,
			0,
			0,
			0
		},
		{
			3,
			1,
			3,
			0,
			0
		}
	};

	GLushort index[] = {
		0, 7, 2,
		5, 6, 7
	};

	glBindVertexArray(ow->vao[0]);
	glBindBuffer(GL_ARRAY_BUFFER, ow->vbo[0]);
	glBufferData(GL_ARRAY_BUFFER, sizeof(pos), pos, GL_STATIC_DRAW);
	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2*sizeof(GLfloat), (GLvoid *)0);
	glEnableVertexAttribArray(0);

	glBindBuffer(GL_DRAW_INDIRECT_BUFFER, ow->vbo[1]);
#if _enable_multi_draw_indirect_
	glBufferData(GL_DRAW_INDIRECT_BUFFER, sizeof(draw_indirect_cmd), draw_indirect_cmd, GL_STATIC_DRAW);
#else
	glBufferData(GL_DRAW_INDIRECT_BUFFER, sizeof(draw_indirect_cmd[0]), &draw_indirect_cmd[0], GL_STATIC_DRAW);
#endif

	glBindVertexArray(ow->vao[1]);
	glBindBuffer(GL_ARRAY_BUFFER, ow->vbo[0]);
	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2*sizeof(GLfloat), (GLvoid *)0);
	glEnableVertexAttribArray(0);

	glBindBuffer(GL_DRAW_INDIRECT_BUFFER, ow->vbo[2]);
#if _enable_multi_draw_indirect_
	glBufferData(GL_DRAW_INDIRECT_BUFFER, sizeof(draw_element_indirect_cmd), draw_element_indirect_cmd, GL_STATIC_DRAW);
#else
	glBufferData(GL_DRAW_INDIRECT_BUFFER, sizeof(draw_element_indirect_cmd[0]), draw_element_indirect_cmd[0], GL_STATIC_DRAW);
#endif

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ow->vbo[3]);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(index), index, GL_STATIC_DRAW);

	while (!glfwWindowShouldClose(ow->win)) {
			glfwPollEvents();
			glClearColor(0.0, 0.0, 0.0, 1.0);
			glClear(GL_COLOR_BUFFER_BIT);

			// indirect draw array
			glUniform4f(glGetUniformLocation(ow->programs[0], "uColor"), 1.0, 0.0, 0.0, 1.0);
			glBindVertexArray(ow->vao[0]);
			glBindBuffer(GL_DRAW_INDIRECT_BUFFER, ow->vbo[1]);
#if _enable_multi_draw_indirect_
			glMultiDrawArraysIndirect(GL_TRIANGLES, (GLvoid *)0, sizeof(draw_indirect_cmd)/sizeof(draw_indirect_cmd[0]), 0);
#else
			glDrawArraysIndirect(GL_TRIANGLES, (GLvoid *)0);
#endif

			// indirec draw element array
			glUniform4f(glGetUniformLocation(ow->programs[0], "uColor"), 0.0, 1.0, 0.0, 1.0);
			glBindVertexArray(ow->vao[1]);
			glBindBuffer(GL_DRAW_INDIRECT_BUFFER, ow->vbo[2]);
#if _enable_multi_draw_indirect_
			glMultiDrawElementsIndirect(GL_TRIANGLES, GL_UNSIGNED_SHORT, (GLvoid *)0, sizeof(draw_element_indirect_cmd)/sizeof(draw_element_indirect_cmd[0]), 0);
#else
			glDrawElementsIndirect(GL_TRIANGLES, GL_UNSIGNED_SHORT, (GLvoid *)0);
#endif

			glfwSwapBuffers(ow->win);
	}

	glfwTerminate();

	return 0;
}
