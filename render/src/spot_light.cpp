#include <glm/glm.hpp>
#include <glm/gtx/transform.hpp>
#include <glm/gtx/rotate_vector.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <SOIL/SOIL.h>
#include "ogl_warp.h"

#define _constant_normal_direction_ 0

GLfloat ground[] = {
	-5.0, 0.0, -5.0,
	5.0, 0.0, -5.0,
	5.0, 0.0, 5.0,
	-5.0, 0.0, -5.0,
	5.0, 0.0, 5.0,
	-5.0, 0.0, 5.0
};

GLfloat texcoord[] = {
	0.0, 1.0,
	1.0, 1.0,
	1.0, 0.0,
	0.0, 1.0,
	1.0, 0.0,
	0.0, 0.0
};

#if _constant_normal_direction_
GLfloat normal[] = {
	0.0, 1.0, 0.0,
	0.0, 1.0, 0.0,
	0.0, 1.0, 0.0,
	0.0, 1.0, 0.0,
	0.0, 1.0, 0.0,
	0.0, 1.0, 0.0
};
#else
GLfloat normal[] = {
	-0.1, 1.0, -0.1,
	0.1, 1.0, -0.1,
	0.1, 1.0, 0.1,
	-0.1, 1.0, -0.1,
	0.1, 1.0, 0.1,
	-0.1, 1.0, 0.1
};
#endif

static float roam_x = 0.0;
static float roam_z = 0.0;
static float roam_y = 0.0;

void self_key_cb(GLFWwindow* win, int key, int scancode, int action, int mode)
{
	if ((key == GLFW_KEY_ESCAPE) && action == GLFW_PRESS)
		glfwSetWindowShouldClose(win, GL_TRUE);

	if ((key == GLFW_KEY_A) && action != GLFW_RELEASE)
		roam_x += 0.1;

	if ((key == GLFW_KEY_D) && action != GLFW_RELEASE)
		roam_x -= 0.1;

	if ((key == GLFW_KEY_W) && action != GLFW_RELEASE)
		roam_z += 0.1;

	if ((key == GLFW_KEY_S) && action != GLFW_RELEASE)
		roam_z -= 0.1;

	if ((key == GLFW_KEY_UP) && action != GLFW_RELEASE)
		roam_y += 0.1;

	if ((key == GLFW_KEY_DOWN) && action != GLFW_RELEASE)
		roam_y -= 0.1;

	if ((key == GLFW_KEY_SPACE) && action == GLFW_PRESS) {
		roam_x = 0.0;
		roam_z = 0.0;
		roam_y = 0.0;
	}
}

int main()
{
	struct ogl_warp *ow = (struct ogl_warp *)malloc(sizeof(struct ogl_warp));
	memset(ow, 0, sizeof(*ow));

	create_ogl_warp(ow);

	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LESS);

	glfwSetKeyCallback(ow->win, self_key_cb);
	create_ogl_warp_shaders(&ow->vertex_shaders[0], "shaders/point_light.vert",
			&ow->fragment_shaders[0], "shaders/spot_light.frag");

	create_ogl_warp_program(ow->vertex_shaders[0],
			ow->fragment_shaders[0], &ow->programs[0]);

	glBindVertexArray(ow->vao[0]);
	glBindBuffer(GL_ARRAY_BUFFER, ow->vbo[0]);
	glBufferData(GL_ARRAY_BUFFER, sizeof(ground), ground, GL_STATIC_DRAW);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3*sizeof(GLfloat), (GLvoid *)0);
	glEnableVertexAttribArray(0);

	glBindBuffer(GL_ARRAY_BUFFER, ow->vbo[1]);
	glBufferData(GL_ARRAY_BUFFER, sizeof(texcoord), texcoord, GL_STATIC_DRAW);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 2*sizeof(GLfloat), (GLvoid *)0);
	glEnableVertexAttribArray(1);

	glBindBuffer(GL_ARRAY_BUFFER, ow->vbo[2]);
	glBufferData(GL_ARRAY_BUFFER, sizeof(normal), normal, GL_STATIC_DRAW);
	glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 3*sizeof(GLfloat), (GLvoid *)0);
	glEnableVertexAttribArray(2);

	GLuint textureGround;
	glGenTextures(1, &textureGround);
	glBindTexture(GL_TEXTURE_2D, textureGround);

	int w, h;
	unsigned char *img = SOIL_load_image("materials/dark-wood.jpg", &w, &h, 0, SOIL_LOAD_RGBA);

	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, w, h, 0, GL_RGBA, GL_UNSIGNED_BYTE, img);
	SOIL_free_image_data(img);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	glActiveTexture(GL_TEXTURE0);
	glUniform1i(glGetUniformLocation(ow->programs[0], "color2D"), 0);

	glm::vec3 light_loc = glm::vec3(0, 1, 0); // original at +y axis
	glm::vec3 light_antenna = glm::vec3(0, -1, 0); // spot light in -y axis

	glm::mat4 model_mat = glm::mat4(1.0);
	glm::mat4 view_mat = glm::lookAt(
			glm::vec3(0, 2, 4),	// camera location
			glm::vec3(0, 0, -1),
			glm::vec3(0, 1, 0)
	);
	glm::mat4 proj_mat = glm::perspective(glm::radians(60.0f), 1.0f, 0.01f, 150.0f);

	while (!glfwWindowShouldClose(ow->win))
	{
		glfwPollEvents();
		glClearColor(0.0, 0.0, 0.0, 1.0);
		glClearDepthf(1.0);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		// fix point light location, and move ground instead
		model_mat = glm::translate(glm::vec3(roam_x, roam_y, roam_z));

		glUniformMatrix4fv(glGetUniformLocation(ow->programs[0], "uModel"), 1, GL_FALSE, &model_mat[0][0]);
		glUniformMatrix4fv(glGetUniformLocation(ow->programs[0], "uView"), 1, GL_FALSE, &view_mat[0][0]);
		glUniformMatrix4fv(glGetUniformLocation(ow->programs[0], "uProj"), 1, GL_FALSE, &proj_mat[0][0]);

		glUniform3fv(glGetUniformLocation(ow->programs[0], "uLightLoc"), 1, &light_loc[0]);

		glUniform3fv(glGetUniformLocation(ow->programs[0], "uAntenna"), 1, &light_antenna[0]);

		glUniform1f(glGetUniformLocation(ow->programs[0], "uRadians"), 3.1415926/6.0);

		glDrawArrays(GL_TRIANGLES, 0, 6);

		glfwSwapBuffers(ow->win);
	}

	glfwTerminate();

	return 0;
}
