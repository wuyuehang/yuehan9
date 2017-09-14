#include "glrunner.h"
#include <glm/glm.hpp>
#include <glm/gtx/transform.hpp>
#include <glm/gtx/rotate_vector.hpp>
#include <glm/gtc/matrix_transform.hpp>

GLuint VS, GS, FS;

void RenderCB(GlRunner *runner)
{
	glClearColor(0.2, 0.1, 0.2, 0.0);
	glClearDepthf(1.0);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glProgramUniform4f(FS, glGetUniformLocation(FS, "uColor"), 1.0, 1.0, 0.0, 0.0);

	glDrawArrays(GL_TRIANGLES, 0, 3);
}

int main()
{
	GlRunner *runner = new GlRunner(RenderCB);

	glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

	GLuint VAO, VBO;
	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &VBO);

	GLfloat pos[] = {
		-1.0, 1.0,
		-1.0, -1.0,
		1.0, 1.0
	};

	glBindVertexArray(VAO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(pos), pos, GL_STATIC_DRAW);

	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2*sizeof(GLfloat), (GLvoid *)0);

	VS = runner->BuildShaderProgram("shaders/simple.vert", GL_VERTEX_SHADER);
	GS = runner->BuildShaderProgram("shaders/subdivide_plane.geo", GL_GEOMETRY_SHADER);
	FS = runner->BuildShaderProgram("shaders/simple.frag", GL_FRAGMENT_SHADER);
	GLuint PPO = runner->BuildProgramPipeline();

	glUseProgramStages(PPO, GL_VERTEX_SHADER_BIT, VS);
	glUseProgramStages(PPO, GL_GEOMETRY_SHADER_BIT, GS);
	glUseProgramStages(PPO, GL_FRAGMENT_SHADER_BIT, FS);

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

	glProgramUniformMatrix4fv(GS, glGetUniformLocation(GS, "uMVP"), 1, GL_FALSE, &mvp_mat[0][0]);

	runner->OnRender();

	return 0;
}
