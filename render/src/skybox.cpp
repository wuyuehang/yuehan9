#include <math.h>
#include <SOIL/SOIL.h>
#include <glm/glm.hpp>
#include <glm/gtx/transform.hpp>
#include <glm/gtx/rotate_vector.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include "glrunner.h"

GLuint VS;

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

	if ((key == GLFW_KEY_A) && action != GLFW_RELEASE)
		hAngle -= 0.1;

	if ((key == GLFW_KEY_D) && action != GLFW_RELEASE)
		hAngle += 0.1;

	if ((key == GLFW_KEY_S) && action != GLFW_RELEASE)
		vAngle -= 0.1;

	if ((key == GLFW_KEY_W) && action != GLFW_RELEASE)
		vAngle += 0.1;

	if ((key == GLFW_KEY_SPACE) && action == GLFW_PRESS) {
		vAngle = 0;
		hAngle = 0;
	}
}

void RenderCB(GlRunner *runner)
{
	glClearColor(0.0, 0.0, 0.0, 1.0);
	glClearDepthf(1.0);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glm::mat4 view_mat = glm::lookAt(
			glm::vec3(0, 0, 0),
			glm::vec3(cos(vAngle)*cos(hAngle), sin(vAngle)*cos(hAngle), sin(hAngle)),
			glm::vec3(0, 1, 0)
	);

	glProgramUniformMatrix4fv(VS, glGetUniformLocation(VS, "uView"), 1, GL_FALSE, &view_mat[0][0]);

	glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_SHORT, (void *)0);
}

int main()
{
	GlRunner *runner = new GlRunner(RenderCB);

	runner->UpdateKeyboardCB(skybox_key_cb);

	VS = runner->BuildShaderProgram("shaders/skybox.vert", GL_VERTEX_SHADER);
	GLuint FS = runner->BuildShaderProgram("shaders/skybox.frag", GL_FRAGMENT_SHADER);

	GLuint pipe = runner->BuildProgramPipeline();

	glUseProgramStages(pipe, GL_VERTEX_SHADER_BIT, VS);
	glUseProgramStages(pipe, GL_FRAGMENT_SHADER_BIT, FS);

	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LEQUAL); // optimize for last draw of skybox

	GLuint VAO;
	glGenVertexArrays(1, &VAO);
	glBindVertexArray(VAO);

	GLuint VBO[2];
	glGenBuffers(2, VBO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO[0]);
	glBufferData(GL_ARRAY_BUFFER, sizeof(cube), cube, GL_STATIC_DRAW);

	glEnableVertexAttribArray(0);
	glVertexAttribFormat(0, 3, GL_FLOAT, GL_FALSE, 0);
	glVertexAttribBinding(0, 0);
	glBindVertexBuffer(0, VBO[0], 0, 3*sizeof(GLfloat));

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, VBO[1]);
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
	glProgramUniform1i(FS, glGetUniformLocation(FS, "uCubeTetxure"), 0);

	glm::mat4 proj_mat = glm::perspective(glm::radians(60.0f), 1.0f, 0.01f, 100.0f);
	glProgramUniformMatrix4fv(VS, glGetUniformLocation(VS, "uProj"), 1, GL_FALSE, &proj_mat[0][0]);

	runner->OnRender();

	return 0;
}
