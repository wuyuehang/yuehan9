#include <assert.h>
#include <SOIL/SOIL.h>
#include <sys/time.h>
#include <glm/glm.hpp>
#include <glm/gtx/transform.hpp>
#include <glm/gtx/rotate_vector.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include "ogl_warp.h"

static float vAngle = 0.0;
static float hAngle = 0.0;

void skybox_key_cb(GLFWwindow* win, int key, int scancode, int action, int mode)
{
	if ((key == GLFW_KEY_ESCAPE) && action == GLFW_PRESS)
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

GLfloat obj[] = {
	-0.8, 0.8,
	0.8, 0.8,
	0.8, -0.8,
	-0.8, 0.8,
	0.8, -0.8,
	-0.8, -0.8
};

GLfloat normal[] = {
	0.0, 0.0, 1.0,
	0.0, 0.0, 1.0,
	0.0, 0.0, 1.0,
	0.0, 0.0, 1.0,
	0.0, 0.0, 1.0,
	0.0, 0.0, 1.0
};

GLfloat skybox[] = {
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

int main()
{
	struct ogl_warp *ow = (struct ogl_warp *)malloc(sizeof(struct ogl_warp));
	memset(ow, 0, sizeof(*ow));

	create_ogl_warp(ow);

	glfwSetKeyCallback(ow->win, skybox_key_cb);

	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LEQUAL); // optimize for last draw of skybox

	// 1. set up resource for the object
	create_ogl_warp_shaders(&ow->vertex_shaders[0], "shaders/environment.vert",
			&ow->fragment_shaders[0], "shaders/environment.frag");

	create_ogl_warp_program(ow->vertex_shaders[0],
			ow->fragment_shaders[0], &ow->programs[0]);

	glm::mat4 model_mat = glm::mat4(1.0);

	glm::mat4 proj_mat = glm::perspective(glm::radians(60.0f), 1.0f, 0.01f, 100.0f);

	glm::vec3 camera_loc = glm::vec3(-2, 0, -2);
	glUniform3fv(glGetUniformLocation(ow->programs[0], "uCameraLoc"), 1, &camera_loc[0]);

	glm::mat4 view_mat = glm::lookAt(
			camera_loc,
			glm::vec3(0, 0, 0),
			glm::vec3(0, 1, 0)
	);

	glBindVertexArray(ow->vao[0]);
	glBindBuffer(GL_ARRAY_BUFFER, ow->vbo[0]);
	glBufferData(GL_ARRAY_BUFFER, sizeof(obj), obj, GL_STATIC_DRAW);
	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2*sizeof(GLfloat), (GLvoid *)0);
	glEnableVertexAttribArray(0);

	glBindBuffer(GL_ARRAY_BUFFER, ow->vbo[3]);
	glBufferData(GL_ARRAY_BUFFER, sizeof(normal), normal, GL_STATIC_DRAW);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 3*sizeof(GLfloat), (GLvoid *)0);
	glEnableVertexAttribArray(1);

	// 2. set up resource for the skybox
	create_ogl_warp_shaders(&ow->vertex_shaders[1], "shaders/skybox.vert",
		&ow->fragment_shaders[1], "shaders/skybox.frag");

	create_ogl_warp_program(ow->vertex_shaders[1],
		ow->fragment_shaders[1], &ow->programs[1]);

	glBindVertexArray(ow->vao[1]);
	glBindBuffer(GL_ARRAY_BUFFER, ow->vbo[1]);
	glBufferData(GL_ARRAY_BUFFER, sizeof(skybox), skybox, GL_STATIC_DRAW);

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3*sizeof(GL_FLOAT), (GLvoid *)0);
	glEnableVertexAttribArray(0);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ow->vbo[2]);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(idx), idx, GL_STATIC_DRAW);

	// load the texture cube map
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
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

		SOIL_free_image_data(img);
	}

	glActiveTexture(GL_TEXTURE0);
	glUniform1i(glGetUniformLocation(ow->programs[1], "uCubeTetxure"), 0);

	glUniformMatrix4fv(glGetUniformLocation(ow->programs[1], "uProj"), 1, GL_FALSE, &proj_mat[0][0]);

	while (!glfwWindowShouldClose(ow->win))
	{
		glfwPollEvents();
		glClearColor(0.0, 0.0, 0.0, 1.0);
		glClearDepthf(1.0);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		// 1. render the object in the skybox
		struct timeval tv;
		gettimeofday(&tv, NULL);
		static long int startup = 0;

		if (0 == startup)
			startup = tv.tv_sec * 1000000 + tv.tv_usec;
		float gap = (tv.tv_sec * 1000000 + tv.tv_usec - startup) / 10000.0f;

		glBindVertexArray(ow->vao[0]);
		glUseProgram(ow->programs[0]);

		glBindTexture(GL_TEXTURE_CUBE_MAP, skyboxID);
		glActiveTexture(GL_TEXTURE0);
		glUniform1i(glGetUniformLocation(ow->programs[0], "uCubeTetxure"), 0);

		model_mat = glm::rotate(glm::mat4(1.0), glm::radians(gap), glm::vec3(0.0, 1.0, 0.0));

		glUniformMatrix4fv(glGetUniformLocation(ow->programs[0], "uModel"), 1, GL_FALSE, &model_mat[0][0]);
		glUniformMatrix4fv(glGetUniformLocation(ow->programs[0], "uView"), 1, GL_FALSE, &view_mat[0][0]);
		glUniformMatrix4fv(glGetUniformLocation(ow->programs[0], "uProj"), 1, GL_FALSE, &proj_mat[0][0]);

		glDrawArrays(GL_TRIANGLES, 0, 6);

		// 2. render the skybox in the background
		glDepthMask(GL_FALSE);

		glBindVertexArray(ow->vao[1]);
		glUseProgram(ow->programs[1]);

		glm::mat4 skybox_view_mat = glm::lookAt(
				glm::vec3(0, 0, 0),
				glm::vec3(cos(vAngle)*cos(hAngle), sin(vAngle)*cos(hAngle), sin(hAngle)),
				glm::vec3(0, 1, 0)
		);

		glUniformMatrix4fv(glGetUniformLocation(ow->programs[1], "uView"), 1, GL_FALSE, &skybox_view_mat[0][0]);
		glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_SHORT, (void *)0);
		glDepthMask(GL_TRUE);
		glfwSwapBuffers(ow->win);
	}

	glfwTerminate();

	return 0;
}
