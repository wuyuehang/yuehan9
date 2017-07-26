#include <glm/glm.hpp>
#include <glm/gtx/transform.hpp>
#include <glm/gtx/rotate_vector.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <SOIL/SOIL.h>
#include "glrunner.h"

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

GLuint VS, FS;

glm::vec3 light_loc = glm::vec3(0, 0.2, 0); // original at +y axis

glm::mat4 model_mat = glm::mat4(1.0);

glm::mat4 view_mat = glm::lookAt(
		glm::vec3(0, 2, 4),	// camera location
		glm::vec3(0, 0, -1),
		glm::vec3(0, 1, 0)
);
glm::mat4 proj_mat = glm::perspective(glm::radians(60.0f), 1.0f, 0.01f, 150.0f);

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

void RenderCB(GlRunner *runner)
{
	glClearColor(0.0, 0.0, 0.0, 1.0);
	glClearDepthf(1.0);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	// fix point light location, and move ground instead
	model_mat = glm::translate(glm::vec3(roam_x, roam_y, roam_z));

	glProgramUniformMatrix4fv(VS, glGetUniformLocation(VS, "uModel"), 1, GL_FALSE, &model_mat[0][0]);
	glProgramUniformMatrix4fv(VS, glGetUniformLocation(VS, "uView"), 1, GL_FALSE, &view_mat[0][0]);
	glProgramUniformMatrix4fv(VS, glGetUniformLocation(VS, "uProj"), 1, GL_FALSE, &proj_mat[0][0]);

	glProgramUniform3fv(FS, glGetUniformLocation(FS, "uLightLoc"), 1, &light_loc[0]);

	glDrawArrays(GL_TRIANGLES, 0, 6);
}

int main()
{
	GlRunner *runner = new GlRunner(RenderCB);

	runner->UpdateKeyboardCB(self_key_cb);

	VS = runner->BuildShaderProgram("shaders/point_light.vert", GL_VERTEX_SHADER);
	FS = runner->BuildShaderProgram("shaders/point_light.frag", GL_FRAGMENT_SHADER);

	GLuint pipe = runner->BuildProgramPipeline();

	glUseProgramStages(pipe, GL_VERTEX_SHADER_BIT, VS);
	glUseProgramStages(pipe, GL_FRAGMENT_SHADER_BIT, FS);

	GLuint VAO;
	glGenVertexArrays(1, &VAO);

	GLuint VBO[3];
	glGenBuffers(3, VBO);
	glBindVertexArray(VAO);

	glBindBuffer(GL_ARRAY_BUFFER, VBO[0]);
	glBufferData(GL_ARRAY_BUFFER, sizeof(ground), ground, GL_STATIC_DRAW);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3*sizeof(GLfloat), (GLvoid *)0);
	glEnableVertexAttribArray(0);

	glBindBuffer(GL_ARRAY_BUFFER, VBO[1]);
	glBufferData(GL_ARRAY_BUFFER, sizeof(texcoord), texcoord, GL_STATIC_DRAW);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 2*sizeof(GLfloat), (GLvoid *)0);
	glEnableVertexAttribArray(1);

	glBindBuffer(GL_ARRAY_BUFFER, VBO[2]);
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
	glProgramUniform1i(FS, glGetUniformLocation(FS, "color2D"), 0);

	runner->OnRender();

	return 0;
}
