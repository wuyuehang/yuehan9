#include <assert.h>
#include <sys/time.h>
#include <glm/glm.hpp>
#include <glm/gtx/transform.hpp>
#include <glm/gtx/rotate_vector.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include "glrunner.h"

GLfloat pos_buf[] = {
		// top face
		-0.5f, 0.5f, -0.5f,
		0.5f, 0.5f, -0.5f,
		-0.5f, 0.5f, 0.5f,

		-0.5f, 0.5f, 0.5f,
		0.5f, 0.5f, -0.5f,
		0.5f, 0.5f, 0.5f,

		// bottom face
		-0.5f, -0.5f, -0.5f,
		-0.5f, -0.5f, 0.5f,
		0.5f, -0.5f, -0.5f,

		0.5f, -0.5f, -0.5f,
		-0.5f, -0.5f, 0.5f,
		0.5f, -0.5f, 0.5f,

		// front face
		-0.5f, 0.5f, 0.5f,
		0.5f, 0.5f, 0.5f,
		-0.5f, -0.5f, 0.5f,

		-0.5f, -0.5f, 0.5f,
		0.5f, 0.5f, 0.5f,
		0.5f, -0.5f, 0.5f,

		// back face
		-0.5f, 0.5f, -0.5f,
		-0.5f, -0.5f, -0.5f,
		0.5f, 0.5f, -0.5f,

		0.5f, 0.5f, -0.5f,
		-0.5f, -0.5f, -0.5f,
		0.5f, -0.5f, -0.5f,

		// left face
		-0.5f, 0.5f, 0.5f,
		-0.5f, -0.5f, 0.5f,
		-0.5f, -0.5f, -0.5f,

		-0.5f, 0.5f, 0.5f,
		-0.5f, -0.5f, -0.5f,
		-0.5f, 0.5f, -0.5f,

		// right face
		0.5f, 0.5f, 0.5f,
		0.5f, -0.5f, -0.5f,
		0.5f, -0.5f, 0.5f,

		0.5f, 0.5f, 0.5f,
		0.5f, 0.5f, -0.5f,
		0.5f, -0.5f, -0.5f,
};

GLfloat normal_buf[] = {
		// top
		0.0f, 1.0f, 0.0f,
		0.0f, 1.0f, 0.0f,
		0.0f, 1.0f, 0.0f,
		0.0f, 1.0f, 0.0f,
		0.0f, 1.0f, 0.0f,
		0.0f, 1.0f, 0.0f,
		// bottom
		0.0f, -1.0f, 0.0f,
		0.0f, -1.0f, 0.0f,
		0.0f, -1.0f, 0.0f,
		0.0f, -1.0f, 0.0f,
		0.0f, -1.0f, 0.0f,
		0.0f, -1.0f, 0.0f,
		// front
		0.0f, 0.0f, 1.0f,
		0.0f, 0.0f, 1.0f,
		0.0f, 0.0f, 1.0f,
		0.0f, 0.0f, 1.0f,
		0.0f, 0.0f, 1.0f,
		0.0f, 0.0f, 1.0f,
		// back
		0.0f, 0.0f, -1.0f,
		0.0f, 0.0f, -1.0f,
		0.0f, 0.0f, -1.0f,
		0.0f, 0.0f, -1.0f,
		0.0f, 0.0f, -1.0f,
		0.0f, 0.0f, -1.0f,
		// left
		-1.0f, 0.0f, 0.0f,
		-1.0f, 0.0f, 0.0f,
		-1.0f, 0.0f, 0.0f,
		-1.0f, 0.0f, 0.0f,
		-1.0f, 0.0f, 0.0f,
		-1.0f, 0.0f, 0.0f,
		// right
		1.0f, 0.0f, 0.0f,
		1.0f, 0.0f, 0.0f,
		1.0f, 0.0f, 0.0f,
		1.0f, 0.0f, 0.0f,
		1.0f, 0.0f, 0.0f,
		1.0f, 0.0f, 0.0f,
};

GLuint VS, FS[2];
GLuint pipe;

GLuint model_uni;
GLuint light_uni;

glm::mat4 view_mat;

void RenderCB(GlRunner *runner)
{
	glClearColor(0.05, 0.05, 0.05, 1.0);
	glClearDepthf(1.0);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	/* calculate time delta */
	struct timeval tv;
	gettimeofday(&tv, nullptr);
	static long int startup = 0;

	if (0 == startup)
		startup = tv.tv_sec * 1000000 + tv.tv_usec;
	float gap = (tv.tv_sec * 1000000 + tv.tv_usec - startup) / 10000.0f;

	/* setup orbit light, visual light location */
	glm::vec3 light_location(8.0f, 0.0f, 0.0f);
	glm::mat4 scale_mat = glm::scale(glm::vec3(0.25f, 0.25f, 0.25f));
	glm::mat4 rotate_mat = glm::rotate(glm::mat4(1.0f), glm::radians(gap), glm::vec3(0.0f, 1.0f, 0.0f));
	glm::mat4 translate_mat = translate(glm::vec3(2.0f, 0.0f, 0.0f));
	glm::mat4 model_mat = rotate_mat * translate_mat * scale_mat;

	// hide light, since we're focusing on rim-light
#if 0
	glUseProgramStages(pipe, GL_FRAGMENT_SHADER_BIT, FS[1]);
	glProgramUniformMatrix4fv(VS, model_uni, 1, GL_FALSE, &model_mat[0][0]);
	glProgramUniform4f(FS[1], glGetUniformLocation(FS[1], "uColor"), 8.0f, 8.0f, 0.0f, 1.0f);
	glDrawArrays(GL_TRIANGLES, 0, 36);
#endif

	light_location = glm::mat3(rotate_mat) * light_location;

	/* setup cube box */
	scale_mat = glm::mat4(1.0f);
	rotate_mat = glm::mat4(1.0f);
	model_mat = rotate_mat * scale_mat;

	glUseProgramStages(pipe, GL_FRAGMENT_SHADER_BIT, FS[0]);
	glProgramUniform3fv(FS[0], light_uni, 1, &light_location[0]);
	glProgramUniformMatrix4fv(VS, model_uni, 1, GL_FALSE, &model_mat[0][0]);
	glDrawArrays(GL_TRIANGLES, 0, 36);
}

int main(int argc, char **argv)
{
	GlRunner *runner = new GlRunner(RenderCB);

	VS = runner->BuildShaderProgram("shaders/phong.vert", GL_VERTEX_SHADER);
	FS[0] = runner->BuildShaderProgram("shaders/rim-light.frag", GL_FRAGMENT_SHADER);

	FS[1] = runner->BuildShaderProgram("shaders/simple.frag", GL_FRAGMENT_SHADER);

	pipe = runner->BuildProgramPipeline();

	glUseProgramStages(pipe, GL_VERTEX_SHADER_BIT, VS);

	GLuint VAO;
	GLuint VBO[2];

	glGenVertexArrays(1, &VAO);
	glBindVertexArray(VAO);

	glGenBuffers(2, VBO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO[0]);
	glBufferData(GL_ARRAY_BUFFER, sizeof(pos_buf), pos_buf, GL_STATIC_DRAW);

	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), (GLvoid *)0);

	glBindBuffer(GL_ARRAY_BUFFER, VBO[1]);
	glBufferData(GL_ARRAY_BUFFER, sizeof(normal_buf), normal_buf, GL_STATIC_DRAW);

	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), (GLvoid *)0);

	model_uni = glGetUniformLocation(VS, "world");
	light_uni= glGetUniformLocation(FS[0], "light_pos");

	/* setup unchanged matrixs and vectors */
	glm::vec3 camera_location(-2, 0.8, 4);

	view_mat = glm::lookAt(
			camera_location,
			glm::vec3(0, 0, 0),
			glm::vec3(0, 1, 0)
	);

	glm::mat4 proj_mat = glm::perspective(glm::radians(60.0f), 1.0f, 0.01f, 100.0f);

	glProgramUniformMatrix4fv(VS, glGetUniformLocation(VS, "view"), 1, GL_FALSE, &view_mat[0][0]);
	glProgramUniformMatrix4fv(VS, glGetUniformLocation(VS, "proj"), 1, GL_FALSE, &proj_mat[0][0]);
	glProgramUniform3fv(FS[0], glGetUniformLocation(FS[0], "view_pos"), 1, &camera_location[0]);

	runner->OnRender();

	return 0;
}
