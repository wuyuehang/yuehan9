#include <math.h>
#include <SOIL/SOIL.h>
#include <glm/glm.hpp>
#include <glm/gtx/transform.hpp>
#include <glm/gtx/rotate_vector.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include "ogl_warp.h"

static float angle = 60.0;
static float ratio = 1.0;
static float near = 0.01;
static float far = 100.0;

void tune_key_cb(GLFWwindow* win, int key, int scancode, int action, int mode)
{
	if ((key == GLFW_KEY_ESCAPE) && action == GLFW_PRESS)
		glfwSetWindowShouldClose(win, GL_TRUE);

	if ((key == GLFW_KEY_A) && action == GLFW_PRESS)
		angle -= 2.5;

	if ((key == GLFW_KEY_D) && action == GLFW_PRESS)
		angle += 2.5;

	if ((key == GLFW_KEY_S) && action == GLFW_PRESS)
		ratio -= 0.01;

	if ((key == GLFW_KEY_W) && action == GLFW_PRESS)
		ratio += 0.01;

	if ((key == GLFW_KEY_RIGHT) && action == GLFW_PRESS)
		near -= 0.05;

	if ((key == GLFW_KEY_LEFT) && action == GLFW_PRESS)
		near += 0.05;

	if ((key == GLFW_KEY_UP) && action == GLFW_PRESS)
		far += 5.0;

	if ((key == GLFW_KEY_DOWN) && action == GLFW_PRESS)
		far -= 5.0;

	if ((key == GLFW_KEY_SPACE) && action == GLFW_PRESS) {
		angle = 60.0;
		ratio = 1.0;
		near = 0.01;
		far = 100.0;
	}

	printf("angle: %f\n", angle);
	printf("ratio: %f\n", ratio);
	printf("near: %f\n", near);
	printf("far: %f\n", far);
}

int main()
{
	struct ogl_warp *ow = (struct ogl_warp *)malloc(sizeof(struct ogl_warp));
	memset(ow, 0, sizeof(*ow));

	create_ogl_warp(ow);

	glfwSetKeyCallback(ow->win, tune_key_cb);

	create_ogl_warp_shaders(&ow->vertex_shaders[0], "shaders/mvp.vert",
			&ow->fragment_shaders[0], "shaders/simple.frag");

	create_ogl_warp_program(ow->vertex_shaders[0],
			ow->fragment_shaders[0], &ow->programs[0]);

	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LESS);

	GLfloat tri0[] = {
		-0.75, 0.75, 0.2,
		0.75, 0.75, 0.2,
		0.75, -0.75, 0.2,
		0.9, 0.9, 0.5,
		0.9, 0.7, 0.5,
		0.7, 0.9, 0.5,
	};

	GLfloat tri1[] = {
		-0.75, 0.75, 0.2,
		0.75, -0.75, 0.2,
		-0.75, -0.75, 0.2,
		0.9, 0.7, 0.5,
		0.7, 0.9, 0.5,
		0.7, 0.7, 0.5
	};

	glBindVertexArray(ow->vao[0]);
	glBindBuffer(GL_ARRAY_BUFFER, ow->vbo[0]);
	glBufferData(GL_ARRAY_BUFFER, sizeof(tri0), tri0, GL_STATIC_DRAW);

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3*sizeof(GL_FLOAT), (GLvoid *)0);
	glEnableVertexAttribArray(0);

	glBindVertexArray(ow->vao[1]);
	glBindBuffer(GL_ARRAY_BUFFER, ow->vbo[1]);
	glBufferData(GL_ARRAY_BUFFER, sizeof(tri1), tri1, GL_STATIC_DRAW);

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3*sizeof(GL_FLOAT), (GLvoid *)0);
	glEnableVertexAttribArray(0);

	glm::mat4 view_mat = glm::lookAt(
			glm::vec3(-3, 1, 4),
			glm::vec3(0, 0, 0),
			glm::vec3(0, 1, 0)
	);

	while (!glfwWindowShouldClose(ow->win)) {
		glfwPollEvents();
		glClearColor(0.0, 0.0, 0.0, 1.0);
		glClearDepthf(1.0);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		glm::mat4 proj_mat = glm::perspective(glm::radians(angle), ratio, near, far);
		glm::mat4 mvp_mat = proj_mat * view_mat;

		glUniformMatrix4fv(glGetUniformLocation(ow->programs[0], "MVP"), 1, GL_FALSE, &mvp_mat[0][0]);

		glBindVertexArray(ow->vao[0]);
		glUniform4f(glGetUniformLocation(ow->programs[0], "uColor"), 1.0, 0.0, 0.0, 1.0);
		glDrawArrays(GL_TRIANGLES, 0, 6);

		glBindVertexArray(ow->vao[1]);
		glUniform4f(glGetUniformLocation(ow->programs[0], "uColor"), 0.0, 0.0, 1.0, 1.0);
		glDrawArrays(GL_TRIANGLES, 0, 6);

		glfwSwapBuffers(ow->win);
	}

	glfwTerminate();

	return 0;
}
