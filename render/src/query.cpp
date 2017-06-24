#include <assert.h>
#include "ogl_warp.h"
#include "utility.h"

GLuint g_uni;
GLuint g_qry;
GLuint g_time_qry;

int main(int argc, char **argv)
{
	struct ogl_warp *ow = (struct ogl_warp *)malloc(sizeof(struct ogl_warp));
	memset(ow, 0, sizeof(*ow));

	create_ogl_warp(ow);

	create_ogl_warp_shaders(&ow->vertex_shaders[0], "shaders/simple.vert",
			&ow->fragment_shaders[0], "shaders/simple.frag");

	create_ogl_warp_program(ow->vertex_shaders[0],
			ow->fragment_shaders[0], &ow->programs[0]);

	// preamble resource
	GLfloat vb_pos[] = {
			// red tri
			-0.5f, -0.5f, 0.3f,
			 0.5f, -0.5f, 0.3f,
			 0.0f,  0.5f, 0.3f,
			 // blue tri
			-0.5f, -0.5f, 0.5f,
			 0.5f, -0.5f, 0.5f,
			 0.0f,  0.5f, 0.5f,
			// green tri
			-0.5f, -0.5f, 0.1f,
			 0.5f, -0.5f, 0.1f,
			 0.0f,  0.5f, 0.1f,
			 // purple tri
			-0.5f, 0.5f, 0.5f,
			0.5f, 0.5f, 0.5f,
			0.0f, -0.5f, 0.5f,
	};

	glBindVertexArray(ow->vao[0]);

	glBindBuffer(GL_ARRAY_BUFFER, ow->vbo[0]);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vb_pos), vb_pos, GL_STATIC_DRAW);

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), (GLvoid*)0);
	glEnableVertexAttribArray(0);

	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LESS);

	glGenQueries(1, &g_time_qry);
	glGenQueries(1, &g_qry);

	{
		glBeginQuery(GL_TIME_ELAPSED, g_time_qry);
		glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
		glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);

		g_uni = glGetUniformLocation(ow->programs[0], "uColor");
		assert(g_uni != 0xFFFFFFFF);

		GLint result;
		glUniform4f(g_uni, 1.0, 0.0, 0.0, 1.0);
		glBeginQuery(GL_SAMPLES_PASSED, g_qry);
		glDrawArrays(GL_TRIANGLES, 0, 3);
		glEndQuery(GL_SAMPLES_PASSED);
		glGetQueryObjectiv(g_qry, GL_QUERY_RESULT, &result);
		printf("1st uncovered triangle, samples result %d\n", result);

		glUniform4f(g_uni, 0.0, 0.0, 1.0, 1.0);
		glBeginQuery(GL_SAMPLES_PASSED, g_qry);
		glDrawArrays(GL_TRIANGLES, 3, 3);
		glEndQuery(GL_SAMPLES_PASSED);
		glGetQueryObjectiv(g_qry, GL_QUERY_RESULT, &result);
		printf("2nd occlusion triangle, samples result %d\n", result);

		glUniform4f(g_uni, 0.0, 0.0, 1.0, 1.0);
		glBeginQuery(GL_SAMPLES_PASSED, g_qry);
		glDrawArrays(GL_TRIANGLES, 6, 3);
		glEndQuery(GL_SAMPLES_PASSED);
		glGetQueryObjectiv(g_qry, GL_QUERY_RESULT, &result);
		printf("3rd uncovered triangle, samples result %d\n", result);

		glUniform4f(g_uni, 1.0, 0.0, 1.0, 1.0);
		glBeginQuery(GL_SAMPLES_PASSED, g_qry);
		glDrawArrays(GL_TRIANGLES, 9, 3);
		glEndQuery(GL_SAMPLES_PASSED);
		glGetQueryObjectiv(g_qry, GL_QUERY_RESULT, &result);
		printf("4th partial uncovered triangle, samples result %d\n", result);

		// test GL_ANY_SAMPLES_PASSED
		GLuint qry;
		glGenQueries(1, &qry);
		glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
		glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);

		glUniform4f(g_uni, 1.0, 0.0, 0.0, 1.0);
		glBeginQuery(GL_ANY_SAMPLES_PASSED, qry);
		glDrawArrays(GL_TRIANGLES, 0, 3);
		glEndQuery(GL_ANY_SAMPLES_PASSED);
		glGetQueryObjectiv(qry, GL_QUERY_RESULT, &result);
		printf("anycover_test, 1st uncovered triangle, samples result %d\n", result);

		glUniform4f(g_uni, 0.0, 0.0, 1.0, 1.0);
		glBeginQuery(GL_ANY_SAMPLES_PASSED, qry);
		glDrawArrays(GL_TRIANGLES, 3, 3);
		glEndQuery(GL_ANY_SAMPLES_PASSED);
		glGetQueryObjectiv(qry, GL_QUERY_RESULT, &result);
		printf("anycover_test, 2nd occlusion triangle, samples result %d\n", result);
		glEndQuery(GL_TIME_ELAPSED);

		glGetQueryObjectiv(g_time_qry, GL_QUERY_RESULT, &result);
		printf("time elapsed, result %d\n", result);
	}

	glfwTerminate();

	return 0;
}
