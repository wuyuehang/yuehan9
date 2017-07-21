#include <glm/glm.hpp>
#include <glm/gtx/transform.hpp>
#include <glm/gtx/rotate_vector.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include "ogl_warp.h"
#include "mesh_loader.h"

#define CameraGrid	float(0.5)

// light property
glm::vec3 LightLocation = glm::vec3(0.0, 4.0, 4.0);

// camera roaming
glm::vec3 RoamCameraLoc = glm::vec3(0, 0, 5);
glm::vec3 RoamLensDirect = glm::vec3(0, 0, -1);
glm::vec3 RoamCameraUp = glm::vec3(0, 1, 0);

float HorizontalDelta = 0.0;
float VerticalDelta = 0.0;
float FOV = 45.0;
float Ratio = 1.0;
float Near = 0.01;
float Far = 100.0;

void model_key_cb(GLFWwindow* win, int key, int scancode, int action, int mode);

int main()
{
	struct ogl_warp *ow = (struct ogl_warp *)malloc(sizeof(struct ogl_warp));

	memset(ow, 0, sizeof(*ow));

	glfwWindowHint(GLFW_SAMPLES, 16);

	create_ogl_warp(ow);

	glfwSetKeyCallback(ow->win, model_key_cb);

	// preamble
	glEnable(GL_MULTISAMPLE);
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LESS);

	create_ogl_warp_shaders(&ow->vertex_shaders[0], "shaders/model.vert",
			&ow->fragment_shaders[0], "shaders/model.frag");

	create_ogl_warp_program(ow->vertex_shaders[0],
			ow->fragment_shaders[0], &ow->programs[0]);

	Mesh *pMeshContainer = new Mesh();

	pMeshContainer->LoadMesh("objs/lantern.obj");

	while (!glfwWindowShouldClose(ow->win)) {
		glfwPollEvents();

		glClearColor(0.2, 0.1, 0.2, 1.0);
		glClearDepthf(1.0);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		// prepare matrix
		glm::mat4 Model = glm::mat4(1.0);
		glm::mat4 ModelTranslation = glm::translate(glm::vec3(HorizontalDelta, VerticalDelta, 0));

		Model = ModelTranslation * Model;

		glm::mat4 View = glm::lookAt(
				RoamCameraLoc, RoamLensDirect, RoamCameraUp);

		glm::mat4 Projection = glm::perspective(glm::radians(FOV), Ratio, Near, Far);

		// update uniform per frame
		glUniformMatrix4fv(glGetUniformLocation(ow->programs[0], "uModel"), 1, GL_FALSE, &Model[0][0]);
		glUniformMatrix4fv(glGetUniformLocation(ow->programs[0], "uView"), 1, GL_FALSE, &View[0][0]);
		glUniformMatrix4fv(glGetUniformLocation(ow->programs[0], "uProjection"), 1, GL_FALSE, &Projection[0][0]);

		// install light
		glUniform3fv(glGetUniformLocation(ow->programs[0], "uLightLoc"), 1, &LightLocation[0]);

		// trigger draw
		pMeshContainer->RenderMesh();

		glfwSwapBuffers(ow->win);
	}

	glfwTerminate();

	return 0;
}

void model_key_cb(GLFWwindow* win, int key, int scancode, int action, int mode)
{
	if ((key == GLFW_KEY_ESCAPE) && action == GLFW_PRESS)
		glfwSetWindowShouldClose(win, GL_TRUE);

	if ((key == GLFW_KEY_A) && action != GLFW_RELEASE)
		RoamCameraLoc += glm::normalize(glm::cross(RoamCameraUp, RoamLensDirect)) * CameraGrid;

	if ((key == GLFW_KEY_D) && action != GLFW_RELEASE)
		RoamCameraLoc -= glm::normalize(glm::cross(RoamCameraUp, RoamLensDirect)) * CameraGrid;

	if ((key == GLFW_KEY_S) && action != GLFW_RELEASE)
		RoamCameraLoc -= RoamLensDirect * CameraGrid;

	if ((key == GLFW_KEY_W) && action != GLFW_RELEASE)
		RoamCameraLoc += RoamLensDirect * CameraGrid;

	if ((key == GLFW_KEY_UP) && action != GLFW_RELEASE)
		VerticalDelta += 0.1;

	if ((key == GLFW_KEY_DOWN) && action != GLFW_RELEASE)
		VerticalDelta -= 0.1;

	if ((key == GLFW_KEY_LEFT) && action != GLFW_RELEASE)
		HorizontalDelta -= 0.1;

	if ((key == GLFW_KEY_RIGHT) && action != GLFW_RELEASE)
		HorizontalDelta += 0.1;

	if ((key == GLFW_KEY_Q) && action != GLFW_RELEASE)
		RoamCameraLoc -= RoamCameraUp * CameraGrid;

	if ((key == GLFW_KEY_E) && action != GLFW_RELEASE)
		RoamCameraLoc += RoamCameraUp * CameraGrid;

	if ((key == GLFW_KEY_SPACE) && action == GLFW_PRESS) {
		RoamCameraLoc = glm::vec3(0, 0, 5);
		RoamLensDirect = glm::vec3(0, 0, -1);
	}
	// graphics mode
	if ((key == GLFW_KEY_1) && action == GLFW_PRESS) {
		static bool wireframe = true;

		if (wireframe) {
			glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
		} else {
			glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
		}
		wireframe = !wireframe;
	}

	if ((key == GLFW_KEY_2) && action == GLFW_PRESS) {
		static bool cullface = true;

		if (cullface) {
			glEnable(GL_CULL_FACE);
			glCullFace(GL_BACK);
			glFrontFace(GL_CCW);
		} else {
			glDisable(GL_CULL_FACE);
		}
		cullface = !cullface;
	}
}
