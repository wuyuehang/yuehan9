#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <cstring>
#include "glrunner.h"

void RenderCB(GlRunner *)
{
	glClearColor(0.1f, 0.1f, 0.1f, 0.0f);
	glClearDepthf(1.0);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glDrawElements(GL_TRIANGLE_STRIP, 4, GL_UNSIGNED_SHORT, (void*)0);
}

int main(int argc, char **argv)
{
	GlRunner *runner = new GlRunner(RenderCB);

	GLuint vs = runner->BuildShaderProgram("shaders/uniform_block.vert", GL_VERTEX_SHADER);
	GLuint fs = runner->BuildShaderProgram("shaders/attrib2.frag", GL_FRAGMENT_SHADER);

	GLuint ppline = runner->BuildProgramPipeline();

	glUseProgramStages(ppline, GL_VERTEX_SHADER_BIT, vs);
	glUseProgramStages(ppline, GL_FRAGMENT_SHADER_BIT, fs);

	// preamble resource
	GLfloat pos_buf[] = {
			-0.25f, 0.25f,
			0.25f, -0.25f,
			-0.25f, -0.25f,
			0.25f, 0.25f
	};

	GLfloat color_buf[] = {
			1.0f, 0.0f, 0.0f,
			0.0f, 1.0f, 0.0f,
			0.0f, 0.0f, 1.0f,
			1.0f, 1.0f, 0.0f
	};

	GLushort index_buf[] = {
			2, 0, 1, 3
	};

	GLuint VAO, VBO[4];
	glGenVertexArrays(1, &VAO);
	glBindVertexArray(VAO);

	glGenBuffers(4, VBO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO[0]);
	glBufferData(GL_ARRAY_BUFFER, sizeof(pos_buf), pos_buf, GL_STATIC_DRAW);

	glBindBuffer(GL_ARRAY_BUFFER, VBO[1]);
	glBufferData(GL_ARRAY_BUFFER, sizeof(color_buf), color_buf, GL_STATIC_DRAW);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, VBO[2]);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(index_buf), index_buf, GL_STATIC_DRAW);

	// binding attribute
	enum { attr_pos, attr_color };

	glEnableVertexAttribArray(attr_pos);
	glEnableVertexAttribArray(attr_color);

	glVertexAttribFormat(attr_pos, 2, GL_FLOAT, GL_FALSE, 0);
	glVertexAttribFormat(attr_color, 3, GL_FLOAT, GL_FALSE, 0);

	glVertexAttribBinding(attr_pos, 0);
	glVertexAttribBinding(attr_color, 1);

	glBindVertexBuffer(0, VBO[0], 0, 2 * sizeof(GLfloat));
	glBindVertexBuffer(1, VBO[1], 0, 3 * sizeof(GLfloat));

	/* updating uniform block */
	glm::mat4 model_mat = glm::mat4(1.0f);

	glm::mat4 view_mat = glm::lookAt(
			glm::vec3(-2, 1, 2),
			glm::vec3(0, 0, 0),
			glm::vec3(0, 1, 0)
	);

	glm::mat4 proj_mat = glm::perspective(glm::radians(45.0f), 1.0f, 0.1f, 100.0f);

	GLuint ubo_blk_idx = glGetUniformBlockIndex(vs, "MVP");
	GLint ubo_sz;
	glGetActiveUniformBlockiv(vs, ubo_blk_idx, GL_UNIFORM_BLOCK_DATA_SIZE, &ubo_sz);

	const char *names[3] = {"uModel", "uView", "uProj"};
	GLuint indices[3];
	GLint size[3];
	GLint offset[3];
	GLint type[3];

	glGetUniformIndices(vs, 3, names, indices);
	glGetActiveUniformsiv(vs, 3, indices, GL_UNIFORM_OFFSET, offset);
	glGetActiveUniformsiv(vs, 3, indices, GL_UNIFORM_SIZE, size);
	glGetActiveUniformsiv(vs, 3, indices, GL_UNIFORM_TYPE, type);

	/* mat4 here is 4x4=16 GLfloat */
	GLubyte *buffer = (GLubyte *)malloc(ubo_sz);
	std::memcpy(buffer + offset[0], &model_mat[0][0], size[0] * sizeof(GLfloat) * 16);
	std::memcpy(buffer + offset[1], &view_mat[0][0], size[1] * sizeof(GLfloat) * 16);
	std::memcpy(buffer + offset[2], &proj_mat[0][0], size[2] * sizeof(GLfloat) * 16);

	glBindBuffer(GL_UNIFORM_BUFFER, VBO[3]);
	glBufferData(GL_UNIFORM_BUFFER, ubo_sz, buffer, GL_STATIC_DRAW);
	glBindBufferBase(GL_UNIFORM_BUFFER, ubo_blk_idx, VBO[3]);

	runner->OnRender();

	return 0;
}
