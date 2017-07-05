#include <math.h>
#include <SOIL/SOIL.h>
#include <glm/glm.hpp>
#include <glm/gtx/transform.hpp>
#include <glm/gtx/rotate_vector.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include "ogl_warp.h"

GLfloat cube[] = {
	-1.0, 1.0, -1.0,
	 1.0, 1.0, -1.0,
	 1.0, 1.0,  1.0,
	-1, 1,  1,
	-1, -1, -1,
	 1, -1, -1,
	 1, -1,  1,
	-1, -1,  1
};

GLushort idx[] = {
	// up
	0, 1, 2,
	0, 2, 3,
	// down
	4, 5, 6,
	4, 6, 7,
	// left
	0, 4, 3,
	3, 4, 7,
	//right
	1, 5, 2,
	2, 5, 6,
	// back
	0, 1, 5,
	0, 5, 4,
	// front,
	3, 2, 6,
	3, 6, 7
};

static float vAngle = 0.0;
static float hAngle = 0.0;

void skybox_key_cb(GLFWwindow* win, int key, int scancode, int action, int mode)
{
	if ((key & GLFW_KEY_ESCAPE) && action == GLFW_PRESS)
		glfwSetWindowShouldClose(win, GL_TRUE);

	if ((key == GLFW_KEY_A) && action == GLFW_PRESS)
		hAngle -= 0.1;

	if ((key == GLFW_KEY_D) && action == GLFW_PRESS)
		hAngle += 0.1;

	if ((key == GLFW_KEY_S) && action == GLFW_PRESS)
		vAngle -= 0.1;

	if ((key == GLFW_KEY_W) && action == GLFW_PRESS)
		vAngle += 0.1;

	if ((key == GLFW_KEY_SPACE) && action == GLFW_PRESS) {
		vAngle = 0;
		hAngle = 0;
	}
}

int main()
{
	struct ogl_warp *ow = (struct ogl_warp *)malloc(sizeof(struct ogl_warp));
	memset(ow, 0, sizeof(*ow));

	create_ogl_warp(ow);

	glfwSetKeyCallback(ow->win, skybox_key_cb);

	create_ogl_warp_shaders(&ow->vertex_shaders[0], "shaders/skybox.vert",
		&ow->fragment_shaders[0], "shaders/skybox.frag");

	create_ogl_warp_program(ow->vertex_shaders[0],
		ow->fragment_shaders[0], &ow->programs[0]);

	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LESS);

	glBindVertexArray(ow->vao[0]);
	glBindBuffer(GL_ARRAY_BUFFER, ow->vbo[0]);
	glBufferData(GL_ARRAY_BUFFER, sizeof(cube), cube, GL_STATIC_DRAW);

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3*sizeof(GL_FLOAT), (GLvoid *)0);
	glEnableVertexAttribArray(0);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ow->vbo[1]);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(idx), idx, GL_STATIC_DRAW);

	// bind cubemap texture
	GLuint skyboxID;
	glGenTextures(1, &skyboxID);
	glBindTexture(GL_TEXTURE_CUBE_MAP, skyboxID);

	const char *skybox_filename[] = {
		"materials/right.jpg",
		"materials/left.jpg",
		"materials/top.jpg",
		"materials/bottom.jpg",
		"materials/back.jpg",
		"materials/front.jpg"
	};

	int w, h;
	unsigned char *img;
	for (int i = 0; i < 6; i++) {
		img = SOIL_load_image(skybox_filename[i], &w, &h, 0, SOIL_LOAD_RGBA);
		glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGBA, w, h, 0, GL_RGBA, GL_UNSIGNED_BYTE, img);
		SOIL_free_image_data(img);
	}

	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

	glActiveTexture(GL_TEXTURE0);
	glUniform1i(glGetUniformLocation(ow->programs[0], "uCubeTetxure"), 0);

	glm::mat4 proj_mat = glm::perspective(glm::radians(60.0f), 1.0f, 0.01f, 100.0f);
	glUniformMatrix4fv(glGetUniformLocation(ow->programs[0], "uProj"), 1, GL_FALSE, &proj_mat[0][0]);


	while (!glfwWindowShouldClose(ow->win)) {
		glfwPollEvents();

		glClearColor(0.0, 0.0, 0.0, 1.0);
		glClearDepthf(1.0);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		glm::mat4 view_mat = glm::lookAt(
				glm::vec3(0, 0, 0),
				glm::vec3(cos(vAngle)*cos(hAngle), sin(vAngle)*cos(hAngle), sin(hAngle)),
				glm::vec3(0, 1, 0)
		);

		glUniformMatrix4fv(glGetUniformLocation(ow->programs[0], "uView"), 1, GL_FALSE, &view_mat[0][0]);

		glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_SHORT, (void *)0);

		glfwSwapBuffers(ow->win);
	}

	glfwTerminate();

	return 0;
}
