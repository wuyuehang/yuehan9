#include <math.h>
#include <SOIL/SOIL.h>
#include <glm/glm.hpp>
#include <glm/gtx/transform.hpp>
#include <glm/gtx/rotate_vector.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include "glrunner.h"

glm::mat4 view_mat = glm::lookAt(
		glm::vec3(-3, 1, 4),
		glm::vec3(0, 0, 0),
		glm::vec3(0, 1, 0)
);

static float angle = 60.0;
static float ratio = 1.0;
static float near = 0.01;
static float far = 100.0;

GLuint vs, fs;
GLuint VAO;
GLuint VBO;

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

	std::cout << "angle: " << angle << std::endl;
	std::cout << "ratio: " << ratio << std::endl;
	std::cout << "near: " << near << std::endl;
	std::cout << "far: " << far << std::endl;
}

void RenderCB(GlRunner *runner)
{
	glClearColor(0.0, 0.0, 0.0, 1.0);
	glClearDepthf(1.0);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glm::mat4 proj_mat = glm::perspective(glm::radians(angle), ratio, near, far);
	glm::mat4 mvp_mat = proj_mat * view_mat;

	glProgramUniformMatrix4fv(vs, glGetUniformLocation(vs, "MVP"), 1, GL_FALSE, &mvp_mat[0][0]);

	glProgramUniform4f(fs, glGetUniformLocation(fs, "uColor"), 1.0, 0.0, 0.0, 1.0);
	glDrawArrays(GL_TRIANGLES, 0, 6);

	glProgramUniform4f(fs, glGetUniformLocation(fs, "uColor"), 0.0, 0.0, 1.0, 1.0);
	glDrawArrays(GL_TRIANGLES, 6, 12);
}

int main()
{
	GlRunner *runner = new GlRunner(RenderCB);

	runner->UpdateKeyboardCB(tune_key_cb);

	vs = runner->BuildShaderProgram("shaders/mvp.vert", GL_VERTEX_SHADER);

	fs = runner->BuildShaderProgram("shaders/simple.frag", GL_FRAGMENT_SHADER);

	GLuint ppline = runner->BuildProgramPipeline();

	glUseProgramStages(ppline, GL_VERTEX_SHADER_BIT, vs);
	glUseProgramStages(ppline, GL_FRAGMENT_SHADER_BIT, fs);

	GLfloat pos[] = {
		-0.75, 0.75, 0.2,
		0.75, 0.75, 0.2,
		0.75, -0.75, 0.2,
		0.9, 0.9, 0.5,
		0.9, 0.7, 0.5,
		0.7, 0.9, 0.5,
		-0.75, 0.75, 0.2,
		0.75, -0.75, 0.2,
		-0.75, -0.75, 0.2,
		0.9, 0.7, 0.5,
		0.7, 0.9, 0.5,
		0.7, 0.7, 0.5
	};

	glGenVertexArrays(1, &VAO);
	glBindVertexArray(VAO);

	glGenBuffers(1, &VBO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(pos), pos, GL_STATIC_DRAW);

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3*sizeof(GL_FLOAT), (GLvoid *)0);
	glEnableVertexAttribArray(0);

	runner->OnRender();

	return 0;
}
