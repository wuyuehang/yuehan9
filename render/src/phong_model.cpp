#include <assert.h>
#include <sys/time.h>
#include <glm/glm.hpp>
#include <glm/gtx/transform.hpp>
#include <glm/gtx/rotate_vector.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include "ogl_warp.h"

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

GLfloat normal_buf[] = {
		// top
		0.0f, 1.0f, 0.0f,
		0.0f, 1.0f, 0.0f,
		0.0f, 1.0f, 0.0f,
		0.0f, 1.0f, 0.0f,
		0.0f, 1.0f, 0.0f,
		0.0f, 1.0f, 0.0f,
		// bottom
		0.0f, -1.0f, 0.0f,
		0.0f, -1.0f, 0.0f,
		0.0f, -1.0f, 0.0f,
		0.0f, -1.0f, 0.0f,
		0.0f, -1.0f, 0.0f,
		0.0f, -1.0f, 0.0f,
		// front
		0.0f, 0.0f, 1.0f,
		0.0f, 0.0f, 1.0f,
		0.0f, 0.0f, 1.0f,
		0.0f, 0.0f, 1.0f,
		0.0f, 0.0f, 1.0f,
		0.0f, 0.0f, 1.0f,
		// back
		0.0f, 0.0f, -1.0f,
		0.0f, 0.0f, -1.0f,
		0.0f, 0.0f, -1.0f,
		0.0f, 0.0f, -1.0f,
		0.0f, 0.0f, -1.0f,
		0.0f, 0.0f, -1.0f,
		// left
		-1.0f, 0.0f, 0.0f,
		-1.0f, 0.0f, 0.0f,
		-1.0f, 0.0f, 0.0f,
		-1.0f, 0.0f, 0.0f,
		-1.0f, 0.0f, 0.0f,
		-1.0f, 0.0f, 0.0f,
		// right
		1.0f, 0.0f, 0.0f,
		1.0f, 0.0f, 0.0f,
		1.0f, 0.0f, 0.0f,
		1.0f, 0.0f, 0.0f,
		1.0f, 0.0f, 0.0f,
		1.0f, 0.0f, 0.0f,
};

int main(int argc, char **argv)
{
	struct ogl_warp *ow = (struct ogl_warp *)malloc(sizeof(struct ogl_warp));
	memset(ow, 0, sizeof(*ow));

	create_ogl_warp(ow);

	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LESS);

	glBindVertexArray(ow->vao[0]);

	glBindBuffer(GL_ARRAY_BUFFER, ow->vbo[0]);
	glBufferData(GL_ARRAY_BUFFER, sizeof(pos_buf), pos_buf, GL_STATIC_DRAW);

	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), (GLvoid *)0);

	glBindBuffer(GL_ARRAY_BUFFER, ow->vbo[1]);
	glBufferData(GL_ARRAY_BUFFER, sizeof(normal_buf), normal_buf, GL_STATIC_DRAW);

	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), (GLvoid *)0);

	create_ogl_warp_shaders(&ow->vertex_shaders[0], "shaders/phong.vert",
			&ow->fragment_shaders[0], "shaders/phong.frag");

	create_ogl_warp_program(ow->vertex_shaders[0],
			ow->fragment_shaders[0], &ow->programs[0]);

	create_ogl_warp_shaders(&ow->vertex_shaders[1], "shaders/phong.vert",
			&ow->fragment_shaders[1], "shaders/simple.frag");

	create_ogl_warp_program(ow->vertex_shaders[1],
			ow->fragment_shaders[1], &ow->programs[1]);

	/* sanity check symbol */
	GLuint model_uni[2];
	model_uni[0] = glGetUniformLocation(ow->programs[0], "world");
	model_uni[1] = glGetUniformLocation(ow->programs[1], "world");
	assert(model_uni[0] != -1);
	assert(model_uni[1] != -1);

	GLuint view_uni[2];
	view_uni[0] = glGetUniformLocation(ow->programs[0], "view");
	view_uni[1] = glGetUniformLocation(ow->programs[1], "view");
	assert(view_uni[0] != -1);
	assert(view_uni[1] != -1);

	GLuint proj_uni[2];
	proj_uni[0] = glGetUniformLocation(ow->programs[0], "proj");
	proj_uni[1] = glGetUniformLocation(ow->programs[1], "proj");
	assert(proj_uni[0] != -1);
	assert(proj_uni[1] != -1);

	GLuint light_uni;
	light_uni= glGetUniformLocation(ow->programs[0], "light_pos");
	assert(light_uni != -1);

	GLuint camera_uni;
	camera_uni = glGetUniformLocation(ow->programs[0], "view_pos");
	assert(camera_uni != -1);

	/* setup unchanged matrixs and vectors */
	glm::vec3 camera_location(-2, 1, 4);

	glm::mat4 view_mat = glm::lookAt(
			camera_location,
			glm::vec3(0, 0, 0),
			glm::vec3(0, 1, 0)
	);

	glm::mat4 proj_mat = glm::perspective(glm::radians(60.0f), 1.0f, 0.01f, 100.0f);

	glUseProgram(ow->programs[0]);
	glUniformMatrix4fv(view_uni[0], 1, GL_FALSE, &view_mat[0][0]);
	glUniformMatrix4fv(proj_uni[0], 1, GL_FALSE, &proj_mat[0][0]);
	glUniform3fv(camera_uni, 1, &camera_location[0]);

	glUseProgram(ow->programs[1]);
	glUniformMatrix4fv(view_uni[1], 1, GL_FALSE, &view_mat[0][0]);
	glUniformMatrix4fv(proj_uni[1], 1, GL_FALSE, &proj_mat[0][0]);

	/* game loop */
	while (!glfwWindowShouldClose(ow->win))
	{
		glfwPollEvents();
		glClearColor(0.05, 0.05, 0.05, 1.0);
		glClearDepthf(1.0);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		/* calculate time delta */
		struct timeval tv;
		gettimeofday(&tv, NULL);
		static long int startup = 0;

		if (0 == startup)
			startup = tv.tv_sec * 1000000 + tv.tv_usec;
		float gap = (tv.tv_sec * 1000000 + tv.tv_usec - startup) / 10000.0f;

		/* setup orbit light, visual light location */
		glm::vec3 light_location(8.0f, 0.0f, 0.0f);
		glm::mat4 scale_mat = glm::scale(glm::vec3(0.25f, 0.25f, 0.25f));
		glm::mat4 rotate_mat = glm::rotate(glm::mat4(1.0f), glm::radians(gap), glm::vec3(0.0f, 1.0f, 0.0f));
		glm::mat4 translate_mat = translate(glm::vec3(2.0f, 0.0f, 0.0f));
		glm::mat4 model_mat = rotate_mat * translate_mat * scale_mat;

		glUseProgram(ow->programs[1]);
		glUniformMatrix4fv(model_uni[0], 1, GL_FALSE, &model_mat[0][0]);
		glUniform4f(glGetUniformLocation(ow->programs[1], "uColor"), 8.0f, 8.0f, 0.0f, 1.0f);
		glDrawArrays(GL_TRIANGLES, 0, 36);

		light_location = glm::mat3(rotate_mat) * light_location;
		/* setup cube box */
		scale_mat = glm::mat4(1.0f);
		rotate_mat = glm::mat4(1.0f);
		model_mat = rotate_mat * scale_mat;

		glUseProgram(ow->programs[0]);

		glUniform3fv(light_uni, 1, &light_location[0]);
		glUniformMatrix4fv(model_uni[0], 1, GL_FALSE, &model_mat[0][0]);
		glDrawArrays(GL_TRIANGLES, 0, 36);
		glfwSwapBuffers(ow->win);
	}

	glfwTerminate();

	return 0;
}