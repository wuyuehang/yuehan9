#include <SOIL/SOIL.h>
#include <glm/glm.hpp>
#include <glm/gtx/transform.hpp>
#include <glm/gtx/rotate_vector.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include "mesh_loader.h"
#include "glrunner.h"

// model property
static float oRefractionRatio = 1.0 / 1.52;
static bool oFlipMode = GL_FALSE;

// camera roaming
#define CameraGrid	float(0.5)
glm::vec3 RoamCameraLoc = glm::vec3(0, 0, 5);
glm::vec3 RoamLensDirect = glm::vec3(0, 0, -1);
glm::vec3 RoamCameraUp = glm::vec3(0, 1, 0);

float HorizontalDelta = 0.0;
float VerticalDelta = 0.0;
float FOV = 45.0;
float Ratio = 1.0;
float Near = 0.01;
float Far = 100.0;

// background skybox property
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

void model_key_cb(GLFWwindow* win, int key, int scancode, int action, int mode);

GLuint VS[2], FS[2];
GLuint pipe;
Mesh *pMeshContainer;
GLuint VAO;
GLuint skyboxID;

void RenderCB(GlRunner *runner)
{
	glClearColor(0.2, 0.1, 0.2, 1.0);
	glClearDepthf(1.0);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	// 3. render the model for optimization
	glUseProgramStages(pipe, GL_VERTEX_SHADER_BIT, VS[0]);
	glUseProgramStages(pipe, GL_FRAGMENT_SHADER_BIT, FS[0]);

	// 3.1 prepare matrix
	glm::mat4 Model = glm::mat4(1.0);

	glm::mat4 ModelTranslation = glm::translate(glm::vec3(HorizontalDelta, VerticalDelta, 0));

	Model = ModelTranslation * Model;

	glm::mat4 View = glm::lookAt(
			RoamCameraLoc, RoamLensDirect, RoamCameraUp);

	glm::mat4 Projection = glm::perspective(glm::radians(FOV), Ratio, Near, Far);

	// 3.2 choose refraction or reflection
	glProgramUniform1i(FS[0], glGetUniformLocation(FS[0], "uIsRefractionMode"), oFlipMode);
	glProgramUniform1f(FS[0], glGetUniformLocation(FS[0], "uRefractionRatio"), oRefractionRatio);

	glBindTexture(GL_TEXTURE_CUBE_MAP, skyboxID);

	glActiveTexture(GL_TEXTURE0);
	glProgramUniform1i(FS[0], glGetUniformLocation(FS[0], "uCubeTetxure"), 0);

	// 3.3 update uniform per frame
	glProgramUniformMatrix4fv(VS[0], glGetUniformLocation(VS[0], "uModel"), 1, GL_FALSE, &Model[0][0]);
	glProgramUniformMatrix4fv(VS[0], glGetUniformLocation(VS[0], "uView"), 1, GL_FALSE, &View[0][0]);
	glProgramUniformMatrix4fv(VS[0], glGetUniformLocation(VS[0], "uProjection"), 1, GL_FALSE, &Projection[0][0]);

	glProgramUniform3fv(FS[0], glGetUniformLocation(FS[0], "uCameraLoc"), 1, &RoamCameraLoc[0]);

	// 3.4 trigger model draw
	pMeshContainer->RenderMesh();

	// 4.1 render the skybox last
	glDepthMask(GL_FALSE);

	glBindVertexArray(VAO);
	glUseProgramStages(pipe, GL_VERTEX_SHADER_BIT, VS[1]);
	glUseProgramStages(pipe, GL_FRAGMENT_SHADER_BIT, FS[1]);

	// 4.2 update cubemap model-view-perspective
	glm::mat4 SkyboxModel = glm::mat4(1.0);
	glm::mat4 SkyboxView = glm::lookAt(
			glm::vec3(0, 0, 0),
			glm::vec3(1, 0, 1),
			glm::vec3(0, 1, 0)
	);
	glm::mat4 SkyboxProj = glm::perspective(glm::radians(FOV), Ratio, Near, Far);

	glProgramUniformMatrix4fv(VS[1], glGetUniformLocation(VS[1], "uModel"), 1, GL_FALSE, &SkyboxModel[0][0]);
	glProgramUniformMatrix4fv(VS[1], glGetUniformLocation(VS[1], "uView"), 1, GL_FALSE, &SkyboxView[0][0]);
	glProgramUniformMatrix4fv(VS[1], glGetUniformLocation(VS[1], "uProj"), 1, GL_FALSE, &SkyboxProj[0][0]);

	glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_SHORT, (void *)0);

	// restore depth mask before swap
	glDepthMask(GL_TRUE);

}

int main()
{
	GlRunner *runner = new GlRunner(RenderCB);

	runner->UpdateKeyboardCB(model_key_cb);

	pipe = runner->BuildProgramPipeline();

	// preamble
	glDepthFunc(GL_LEQUAL);

	// 1. prepare resource for model
	VS[0] = runner->BuildShaderProgram("shaders/model.vert", GL_VERTEX_SHADER);
	FS[0] = runner->BuildShaderProgram("shaders/model2.frag", GL_FRAGMENT_SHADER);

	pMeshContainer = new Mesh();

	pMeshContainer->LoadMesh("objs/stormtrooper.obj");

	// 2.1 prepare resource for skybox background
	VS[1] = runner->BuildShaderProgram("shaders/skybox.vert", GL_VERTEX_SHADER);
	FS[1] = runner->BuildShaderProgram("shaders/skybox.frag", GL_FRAGMENT_SHADER);

	glGenVertexArrays(1, &VAO);
	glBindVertexArray(VAO);

	GLuint VBO[2];
	glGenBuffers(2, VBO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO[0]);
	glBufferData(GL_ARRAY_BUFFER, sizeof(skybox), skybox, GL_STATIC_DRAW);

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3*sizeof(GL_FLOAT), (GLvoid *)0);
	glEnableVertexAttribArray(0);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, VBO[1]);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(idx), idx, GL_STATIC_DRAW);

	// 2.2 load the texture cube map
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
	glProgramUniform1i(FS[1], glGetUniformLocation(FS[1], "uCubeTetxure"), 0);

	runner->OnRender();

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

	if ((key == GLFW_KEY_3) && action == GLFW_PRESS) {
		oFlipMode = !oFlipMode;
	}
}
