#include <sys/time.h>
#include <glm/glm.hpp>
#include <glm/gtx/transform.hpp>
#include <glm/gtx/rotate_vector.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include "ogl_warp.h"

int main()
{
	struct ogl_warp *ow = (struct ogl_warp *)malloc(sizeof(struct ogl_warp));
	memset(ow, 0, sizeof(*ow));

	create_ogl_warp(ow);

	create_ogl_warp_shaders(&ow->vertex_shaders[0], "shaders/mvp.vert",
			&ow->fragment_shaders[0], "shaders/simple.frag");

	create_ogl_warp_program(ow->vertex_shaders[0],
			ow->fragment_shaders[0], &ow->programs[0]);

	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LESS);

	GLfloat pos_buf[] = {
			// top face
			-0.5f, 0.5f, -0.5f,
			0.5f, 0.5f, -0.5f,
			-0.5f, 0.5f, 0.5f,

			-0.5f, 0.5f, 0.5f,
			0.5f, 0.5f, -0.5f,
			0.5f, 0.5f, 0.5f,

			// bottom face
			-0.5f, -0.5f, -0.5f,
			-0.5f, -0.5f, 0.5f,
			0.5f, -0.5f, -0.5f,

			0.5f, -0.5f, -0.5f,
			-0.5f, -0.5f, 0.5f,
			0.5f, -0.5f, 0.5f,

			// front face
			-0.5f, 0.5f, 0.5f,
			0.5f, 0.5f, 0.5f,
			-0.5f, -0.5f, 0.5f,

			-0.5f, -0.5f, 0.5f,
			0.5f, 0.5f, 0.5f,
			0.5f, -0.5f, 0.5f,

			// back face
			-0.5f, 0.5f, -0.5f,
			-0.5f, -0.5f, -0.5f,
			0.5f, 0.5f, -0.5f,

			0.5f, 0.5f, -0.5f,
			-0.5f, -0.5f, -0.5f,
			0.5f, -0.5f, -0.5f,

			// left face
			-0.5f, 0.5f, 0.5f,
			-0.5f, -0.5f, 0.5f,
			-0.5f, -0.5f, -0.5f,

			-0.5f, 0.5f, 0.5f,
			-0.5f, -0.5f, -0.5f,
			-0.5f, 0.5f, -0.5f,

			// right face
			0.5f, 0.5f, 0.5f,
			0.5f, -0.5f, -0.5f,
			0.5f, -0.5f, 0.5f,

			0.5f, 0.5f, 0.5f,
			0.5f, 0.5f, -0.5f,
			0.5f, -0.5f, -0.5f,
	};

	/* handle 1st vao for object box */
	glBindVertexArray(ow->vao[0]);

	enum { attr_pos };

	glBindBuffer(GL_ARRAY_BUFFER, ow->vbo[0]);
	glBufferData(GL_ARRAY_BUFFER, sizeof(pos_buf), pos_buf, GL_STATIC_DRAW);

	glEnableVertexAttribArray(0);
	glVertexAttribFormat(attr_pos, 3, GL_FLOAT, GL_FALSE, 0);

	glVertexAttribBinding(attr_pos, 0);

	glBindVertexBuffer(0, ow->vbo[0], 0, 3 * sizeof(GLfloat));

	/* handle 2nd vao for light box */
	glBindVertexArray(ow->vao[1]);

	glEnableVertexAttribArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, ow->vbo[0]);
	glBufferData(GL_ARRAY_BUFFER, sizeof(pos_buf), pos_buf, GL_STATIC_DRAW);

	glEnableVertexAttribArray(0);
	glVertexAttribFormat(attr_pos, 3, GL_FLOAT, GL_FALSE, 0);

	glVertexAttribBinding(attr_pos, 0);

	glBindVertexBuffer(0, ow->vbo[0], 0, 3 * sizeof(GLfloat));

	glm::mat4 view_mat = glm::lookAt(
			glm::vec3(-2, 2, 4),
			glm::vec3(0, 0, 0),
			glm::vec3(0, 1, 0)
	);

	glm::mat4 proj_mat = glm::perspective(glm::radians(60.0f), 1.0f, 0.01f, 100.0f);

	while (!glfwWindowShouldClose(ow->win))
	{
		glfwPollEvents();

		glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		/* recalculate model world matrix */
		struct timeval tv;
		gettimeofday(&tv, NULL);
		static long int startup = 0;

		if (0 == startup)
			startup = tv.tv_sec * 1000000 + tv.tv_usec;

		float gap = (tv.tv_sec * 1000000 + tv.tv_usec - startup) / 10000.0f;

		/* prepare matrix for drawing first orbit box */
		glm::mat4 scale_mat = glm::scale(glm::vec3(0.5f, 0.5f, 0.5f));

		glm::mat4 rotate_mat = glm::rotate(glm::mat4(1.0f), glm::radians(gap), glm::vec3(0.0f, 1.0f, 0.0f));

		glm::mat4 translate_mat = glm::translate(glm::vec3(2.0f, 0.0f, 0.0f));

		glm::mat4 world_mat = rotate_mat * translate_mat * scale_mat;

		glm::mat4 mvp_mat = proj_mat * view_mat * world_mat;

		glBindVertexArray(ow->vao[0]);
		GLuint color_uni = glGetUniformLocation(ow->programs[0], "uColor");
		assert(color_uni != 0xFFFFFFFF);

		glUniform4f(color_uni, 1.0, 0.0, 0.0, 1.0);
		GLuint mvp_uni;
		mvp_uni = glGetUniformLocation(ow->programs[0], "MVP");
		assert(mvp_uni != -1);
		glUniformMatrix4fv(mvp_uni, 1, GL_FALSE, &mvp_mat[0][0]);

		glDrawArrays(GL_TRIANGLES, 0, 3 * 36);
		glBindVertexArray(0);

		/* prepare matrix for drawing second rotate box */
		scale_mat = glm::scale(glm::vec3(1.0f, 1.0f, 1.0f));

		rotate_mat = glm::rotate(glm::mat4(1.0f), glm::radians(gap / 2.0f), glm::vec3(1.0f, 0.0f, 0.0f));

		world_mat = rotate_mat * scale_mat;

		mvp_mat = proj_mat * view_mat * world_mat;

		glBindVertexArray(ow->vao[1]);
		glUniformMatrix4fv(mvp_uni, 1, GL_FALSE, &mvp_mat[0][0]);
		glUniform4f(color_uni, 0.0, 1.0, 0.0, 0.0);

		glDrawArrays(GL_TRIANGLES, 0, 3 * 36);
		glBindVertexArray(0);

		glfwSwapBuffers(ow->win);
	}

	glfwTerminate();

	return 0;
}
