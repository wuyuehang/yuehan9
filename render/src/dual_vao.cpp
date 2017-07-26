#include <sys/time.h>
#include <glm/glm.hpp>
#include <glm/gtx/transform.hpp>
#include <glm/gtx/rotate_vector.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include "glrunner.h"

GLuint VAO[2];
GLuint VBO[1];
GLuint vsProgram, fsProgram;

glm::mat4 view_mat = glm::lookAt(
		glm::vec3(-2, 2, 4),
		glm::vec3(0, 0, 0),
		glm::vec3(0, 1, 0)
);

glm::mat4 proj_mat = glm::perspective(glm::radians(60.0f), 1.0f, 0.01f, 100.0f);

void RenderCB(GlRunner *runner)
{
	glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
	glClearDepthf(1.0);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	/* recalculate model world matrix */
	struct timeval tv;
	gettimeofday(&tv, nullptr);
	static long int startup = 0;

	if (0 == startup)
		startup = tv.tv_sec * 1000000 + tv.tv_usec;

	float gap = (tv.tv_sec * 1000000 + tv.tv_usec - startup) / 10000.0f;

	/* prepare matrix for drawing first orbit box */
	glm::mat4 scale_mat = glm::scale(glm::vec3(0.5f, 0.5f, 0.5f));

	glm::mat4 rotate_mat = glm::rotate(glm::mat4(1.0f), glm::radians(gap), glm::vec3(0.0f, 1.0f, 0.0f));

	glm::mat4 translate_mat = glm::translate(glm::vec3(2.0f, 0.0f, 0.0f));

	glm::mat4 world_mat = rotate_mat * translate_mat * scale_mat;

	glm::mat4 mvp_mat = proj_mat * view_mat * world_mat;

	glBindVertexArray(VAO[0]);

	glProgramUniform4f(fsProgram, glGetUniformLocation(fsProgram, "uColor"), 1.0, 0.0, 0.0, 1.0);
	glProgramUniformMatrix4fv(vsProgram, glGetUniformLocation(vsProgram, "MVP"), 1, GL_FALSE, &mvp_mat[0][0]);

	glDrawArrays(GL_TRIANGLES, 0, 3 * 36);

	/* prepare matrix for drawing second rotate box */
	scale_mat = glm::scale(glm::vec3(1.0f, 1.0f, 1.0f));

	rotate_mat = glm::rotate(glm::mat4(1.0f), glm::radians(gap / 2.0f), glm::vec3(1.0f, 0.0f, 0.0f));

	world_mat = rotate_mat * scale_mat;

	mvp_mat = proj_mat * view_mat * world_mat;

	glBindVertexArray(VAO[1]);

	glProgramUniformMatrix4fv(vsProgram, glGetUniformLocation(vsProgram, "MVP"), 1, GL_FALSE, &mvp_mat[0][0]);
	glProgramUniform4f(fsProgram, glGetUniformLocation(fsProgram, "uColor"), 0.0, 1.0, 0.0, 0.0);

	glDrawArrays(GL_TRIANGLES, 0, 3 * 36);
}

int main()
{
	GlRunner *runner = new GlRunner(RenderCB);

	vsProgram = runner->BuildShaderProgram("shaders/mvp.vert", GL_VERTEX_SHADER);
	fsProgram = runner->BuildShaderProgram("shaders/simple.frag", GL_FRAGMENT_SHADER);
	GLuint pipeline = runner->BuildProgramPipeline();

	glUseProgramStages(pipeline, GL_VERTEX_SHADER_BIT, vsProgram);
	glUseProgramStages(pipeline, GL_FRAGMENT_SHADER_BIT, fsProgram);

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

	/* handle 1st vao for object box */
	glGenVertexArrays(2, VAO);
	glBindVertexArray(VAO[0]);

	glGenBuffers(1, VBO);

	enum { attr_pos };

	glBindBuffer(GL_ARRAY_BUFFER, VBO[0]);
	glBufferData(GL_ARRAY_BUFFER, sizeof(pos_buf), pos_buf, GL_STATIC_DRAW);

	glEnableVertexAttribArray(0);
	glVertexAttribFormat(attr_pos, 3, GL_FLOAT, GL_FALSE, 0);

	glVertexAttribBinding(attr_pos, 0);

	glBindVertexBuffer(0, VBO[0], 0, 3 * sizeof(GLfloat));

	/* handle 2nd vao for light box */
	glBindVertexArray(VAO[1]);

	glEnableVertexAttribArray(0);

	glEnableVertexAttribArray(0);
	glVertexAttribFormat(attr_pos, 3, GL_FLOAT, GL_FALSE, 0);

	glVertexAttribBinding(attr_pos, 0);

	glBindVertexBuffer(0, VBO[0], 0, 3 * sizeof(GLfloat));

	runner->OnRender();

	return 0;
}
