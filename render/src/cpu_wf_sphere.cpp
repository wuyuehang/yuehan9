#include "glrunner.h"
#include <vector>
#include <cmath>
#include <glm/glm.hpp>
#include <glm/gtx/transform.hpp>
#include <glm/gtx/rotate_vector.hpp>
#include <glm/gtc/matrix_transform.hpp>

#define _pi	3.1415926
#define _incremental	5

using std::vector;

vector<GLfloat> vertexs;

void RenderCB(GlRunner *runner)
{
	glClearColor(0.2, 0.1, 0.2, 0.0);
	glClearDepthf(1.0);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glDrawArrays(GL_LINE_STRIP, 0, vertexs.size());
}

int main()
{
	GlRunner *runner = new GlRunner(RenderCB);

	GLuint VS = runner->BuildShaderProgram("shaders/cpu_wf_sphere.vert", GL_VERTEX_SHADER);
	GLuint FS = runner->BuildShaderProgram("shaders/simple.frag", GL_FRAGMENT_SHADER);
	GLuint PPO = runner->BuildProgramPipeline();
	glUseProgramStages(PPO, GL_VERTEX_SHADER_BIT, VS);
	glUseProgramStages(PPO, GL_FRAGMENT_SHADER_BIT, FS);

	glProgramUniform4f(FS, glGetUniformLocation(FS, "uColor"), 0.75, 0.0, 0.0, 0.0);

	/* cpu domain generate linestrip constructed sphere
	 * effect as wireframe */
	GLfloat x, y, z;

	/* generate longtitude vertex */
	for (int i = 0; i <= 360; i+= _incremental) {
		for (int j = 0; j <= 180; j+= _incremental) {
			x = cos(i*_pi/180.0) * sin(j*_pi/180.0);
			y = sin(i*_pi/180.0) * sin(j*_pi/180.0);
			z = cos(j*_pi/180.0);
			vertexs.push_back(x);
			vertexs.push_back(y);
			vertexs.push_back(z);
		}
	}

	/* generate latitude vertex */
	for (int j = 0; j <= 180; j+= _incremental) {
		for (int i = 0; i <= 360; i+= _incremental) {
			x = cos(i*_pi/180.0) * sin(j*_pi/180.0);
			y = sin(i*_pi/180.0) * sin(j*_pi/180.0);
			z = cos(j*_pi/180.0);
			vertexs.push_back(x);
			vertexs.push_back(y);
			vertexs.push_back(z);
		}
	}

	/* loading input attribute */
	GLuint VAO, VBO;
	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &VBO);

	glBindVertexArray(VAO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat)*vertexs.size(), &vertexs[0], GL_STATIC_DRAW);

	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3*sizeof(GLfloat), (GLvoid *)0);

	/* setup matrixs */
	glm::mat4 model_mat = glm::scale(glm::vec3(2.5));
	glm::mat4 proj_mat = glm::perspective(glm::radians(75.0f), 1.0f, 0.01f, 100.0f);

	glm::vec3 camera_location(0, 2.5, 5);

	glm::mat4 view_mat = glm::lookAt(
			camera_location,
			glm::vec3(0, 0, 0),
			glm::vec3(0, 1, 0)
	);

	glm::mat4 mvp_mat = proj_mat * view_mat * model_mat;

	glProgramUniformMatrix4fv(VS, glGetUniformLocation(VS, "uMVP"), 1, GL_FALSE, &mvp_mat[0][0]);

	runner->OnRender();

	return 0;
}
