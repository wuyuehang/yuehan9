#include "ogl_warp.h"

#define _map_xfb_for_validation_ 0
#define _query_xfb_written_ 0
#define _xfb_sync_ 1

int main()
{
	struct ogl_warp *ow = (struct ogl_warp *)malloc(sizeof(struct ogl_warp));

	create_ogl_warp(ow);

	glEnable(GL_RASTERIZER_DISCARD);
	// create xfb render pass, translate the original
	// vertice to up-right by one unit
	create_ogl_warp_shaders(&ow->vertex_shaders[0], "shaders/xfb.vert",
			NULL, NULL);

	ow->programs[0] = glCreateProgram();
	glAttachShader(ow->programs[0], ow->vertex_shaders[0]);

	GLchar *xfb_list[] = {"xfb_pos"};
	glTransformFeedbackVaryings(ow->programs[0], 1, xfb_list, GL_INTERLEAVED_ATTRIBS);

	glLinkProgram(ow->programs[0]);
	glUseProgram(ow->programs[0]);

	GLfloat vb_pos[] = {
		-1.0, 0.0,
		-1.0, -1.0,
		0.0, -1.0,
		0.0, 0.0
	};

	glBindVertexArray(ow->vao[0]);

	glBindBuffer(GL_ARRAY_BUFFER, ow->vbo[0]);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vb_pos), vb_pos, GL_STATIC_DRAW);
	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(GLfloat), (GLvoid *)0);
	glEnableVertexAttribArray(0);

	// store the transform feedback
	glBindBuffer(GL_ARRAY_BUFFER, ow->vbo[1]);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vb_pos), NULL, GL_STATIC_READ);
	glBindBufferBase(GL_TRANSFORM_FEEDBACK_BUFFER, 0, ow->vbo[1]);

#if _query_xfb_written_
	GLuint xfb_qry;
	glGenQueries(1, &xfb_qry);

	glBeginQuery(GL_TRANSFORM_FEEDBACK_PRIMITIVES_WRITTEN, xfb_qry);
#endif

	glBeginTransformFeedback(GL_POINTS);
	glDrawArrays(GL_POINTS, 0, 4);
	glEndTransformFeedback();

#if _xfb_sync_
	GLuint xfb_sync = glFenceSync(GL_SYNC_GPU_COMMANDS_COMPLETE, 0);
#endif

#if _query_xfb_written_
	glEndQuery(GL_TRANSFORM_FEEDBACK_PRIMITIVES_WRITTEN);

	GLuint num_primitives;
	glGetQueryObjectuiv(xfb_qry, GL_QUERY_RESULT, &num_primitives);
	printf("xfb writes %d primitives\n", num_primitives);
#endif

#if _map_xfb_for_validation_
	GLfloat result[8];
	glGetBufferSubData(GL_TRANSFORM_FEEDBACK_BUFFER, 0, sizeof(result), result);
	int i = 0;
	for (; i < 4; i++) {
		printf("vertice %d x(%f) y(%f)\n", result[2*i], result[2*i+1]);
	}
#endif

	// draw the xfb vertex
	create_ogl_warp_shaders(&ow->vertex_shaders[1], "shaders/simple.vert",
			&ow->fragment_shaders[1], "shaders/simple.frag");

	create_ogl_warp_program(ow->vertex_shaders[1],
			ow->fragment_shaders[1], &ow->programs[1]);

	glUniform4f(glGetUniformLocation(ow->programs[1], "uColor"), 1.0, 0.0, 0.0, 1.0);

	glBindVertexArray(ow->vao[1]);
	glBindBuffer(GL_ARRAY_BUFFER, ow->vbo[1]);
	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(GLfloat), (GLvoid *)0);
	glEnableVertexAttribArray(0);

	glDisable(GL_RASTERIZER_DISCARD);

#if _xfb_sync_
	glWaitSync(xfb_sync, 0, GL_TIMEOUT_IGNORED);
#endif

	while (!glfwWindowShouldClose(ow->win)) {
		glfwPollEvents();
		glDrawArrays(GL_TRIANGLES, 0, 3);
		glfwSwapBuffers(ow->win);
	}

	glfwTerminate();

	return 0;
}
