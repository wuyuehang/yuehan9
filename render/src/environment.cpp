#include <assert.h>
#include <SOIL/SOIL.h>
#include <sys/time.h>
#include <glm/glm.hpp>
#include <glm/gtx/transform.hpp>
#include <glm/gtx/rotate_vector.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include "glrunner.h"

#define _animate_object_ 1

static float vAngle = 0.0;
static float hAngle = 0.0;
static float oRefractionRatio = 1.0 / 1.52;
static bool oFlipMode = GL_FALSE;

void skybox_key_cb(GLFWwindow* win, int key, int scancode, int action, int mode)
{
	if ((key == GLFW_KEY_ESCAPE) && action == GLFW_PRESS)
		glfwSetWindowShouldClose(win, GL_TRUE);

	if ((key == GLFW_KEY_A) && action != GLFW_RELEASE)
		hAngle -= 0.1;

	if ((key == GLFW_KEY_D) && action != GLFW_RELEASE)
		hAngle += 0.1;

	if ((key == GLFW_KEY_S) && action != GLFW_RELEASE)
		vAngle -= 0.1;

	if ((key == GLFW_KEY_W) && action != GLFW_RELEASE)
		vAngle += 0.1;

	if ((key == GLFW_KEY_UP) && action != GLFW_RELEASE)
		oRefractionRatio += 0.05;

	if ((key == GLFW_KEY_DOWN) && action != GLFW_RELEASE)
		oRefractionRatio -= 0.05;

	if ((key == GLFW_KEY_SPACE) && action == GLFW_PRESS)
		oFlipMode = !oFlipMode;

	if ((key == GLFW_KEY_R) && action == GLFW_PRESS) {
		vAngle = 0;
		hAngle = 0;
		oRefractionRatio = 1.0 / 1.52;
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

GLuint VS[2], FS[2], pipe;
GLuint skyboxID;
GLuint VAO[2];

glm::mat4 proj_mat = glm::perspective(glm::radians(60.0f), 1.0f, 0.01f, 100.0f);

glm::vec3 camera_loc = glm::vec3(-2, 0, -2);

glm::mat4 model_mat = glm::mat4(1.0);

glm::mat4 view_mat = glm::lookAt(
		camera_loc,
		glm::vec3(0, 0, 0),
		glm::vec3(0, 1, 0)
);

void RenderCB(GlRunner *runner)
{
	glClearColor(0.0, 0.0, 0.0, 1.0);
	glClearDepthf(1.0);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	// 1. render the object in the skybox
#if _animate_object_
	struct timeval tv;
	gettimeofday(&tv, nullptr);
	static long int startup = 0;

	if (0 == startup)
		startup = tv.tv_sec * 1000000 + tv.tv_usec;
	float gap = (tv.tv_sec * 1000000 + tv.tv_usec - startup) / 10000.0f;
#endif

	glBindVertexArray(VAO[0]);
	glUseProgramStages(pipe, GL_VERTEX_SHADER_BIT, VS[0]);
	glUseProgramStages(pipe, GL_FRAGMENT_SHADER_BIT, FS[0]);

	glProgramUniform1i(FS[0], glGetUniformLocation(FS[0], "uIsRefractionMode"), oFlipMode);
	glProgramUniform1f(FS[0], glGetUniformLocation(FS[0], "uRefractionRatio"), oRefractionRatio);
	glBindTexture(GL_TEXTURE_CUBE_MAP, skyboxID);
	glActiveTexture(GL_TEXTURE0);
	glProgramUniform1i(FS[0], glGetUniformLocation(FS[0], "uCubeTetxure"), 0);

#if _animate_object_
	model_mat = glm::rotate(glm::mat4(1.0), glm::radians(gap), glm::vec3(0.0, 1.0, 0.0));
#else
	model_mat = glm::mat4(1.0);
#endif

	glProgramUniformMatrix4fv(VS[0], glGetUniformLocation(VS[0], "uModel"), 1, GL_FALSE, &model_mat[0][0]);
	glProgramUniformMatrix4fv(VS[0], glGetUniformLocation(VS[0], "uView"), 1, GL_FALSE, &view_mat[0][0]);
	glProgramUniformMatrix4fv(VS[0], glGetUniformLocation(VS[0], "uProj"), 1, GL_FALSE, &proj_mat[0][0]);

	glDrawArrays(GL_TRIANGLES, 0, 6);

	// 2. render the skybox in the background
	glDepthMask(GL_FALSE);

	glBindVertexArray(VAO[1]);
	glUseProgramStages(pipe, GL_VERTEX_SHADER_BIT, VS[1]);
	glUseProgramStages(pipe, GL_FRAGMENT_SHADER_BIT, FS[1]);

	glm::mat4 skybox_view_mat = glm::lookAt(
			glm::vec3(0, 0, 0),
			glm::vec3(cos(vAngle)*cos(hAngle), sin(vAngle)*cos(hAngle), sin(hAngle)),
			glm::vec3(0, 1, 0)
	);

	glProgramUniformMatrix4fv(VS[1], glGetUniformLocation(VS[1], "uView"), 1, GL_FALSE, &skybox_view_mat[0][0]);
	glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_SHORT, (void *)0);
	glDepthMask(GL_TRUE);
}

int main()
{
	GlRunner *runner = new GlRunner(RenderCB);

	runner->UpdateKeyboardCB(skybox_key_cb);

	glDepthFunc(GL_LEQUAL); // optimize for last draw of skybox

	// 1. set up resource for the object
	VS[0] = runner->BuildShaderProgram("shaders/environment.vert", GL_VERTEX_SHADER);
	FS[0] = runner->BuildShaderProgram("shaders/environment.frag", GL_FRAGMENT_SHADER);

	pipe = runner->BuildProgramPipeline();

	glProgramUniform3fv(FS[0], glGetUniformLocation(FS[0], "uCameraLoc"), 1, &camera_loc[0]);

	glGenVertexArrays(2, VAO);
	glBindVertexArray(VAO[0]);

	GLuint VBO[4];
	glGenBuffers(4, VBO);

	glBindBuffer(GL_ARRAY_BUFFER, VBO[0]);
	glBufferData(GL_ARRAY_BUFFER, sizeof(obj), obj, GL_STATIC_DRAW);
	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2*sizeof(GLfloat), (GLvoid *)0);
	glEnableVertexAttribArray(0);

	glBindBuffer(GL_ARRAY_BUFFER, VBO[3]);
	glBufferData(GL_ARRAY_BUFFER, sizeof(normal), normal, GL_STATIC_DRAW);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 3*sizeof(GLfloat), (GLvoid *)0);
	glEnableVertexAttribArray(1);

	// 2. set up resource for the skybox
	VS[1] = runner->BuildShaderProgram("shaders/skybox.vert", GL_VERTEX_SHADER);
	FS[1] = runner->BuildShaderProgram("shaders/skybox.frag", GL_FRAGMENT_SHADER);

	glBindVertexArray(VAO[1]);
	glBindBuffer(GL_ARRAY_BUFFER, VBO[1]);
	glBufferData(GL_ARRAY_BUFFER, sizeof(skybox), skybox, GL_STATIC_DRAW);

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3*sizeof(GL_FLOAT), (GLvoid *)0);
	glEnableVertexAttribArray(0);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, VBO[2]);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(idx), idx, GL_STATIC_DRAW);

	// load the texture cube map
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

	glProgramUniformMatrix4fv(VS[1], glGetUniformLocation(VS[1], "uProj"), 1, GL_FALSE, &proj_mat[0][0]);

	runner->OnRender();

	return 0;
}
