#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include "ogl_warp.h"

int main(int argc, char **argv)
{
	struct ogl_warp *ow = (struct ogl_warp *)malloc(sizeof(ogl_warp));
	memset(ow, 0, sizeof(*ow));

	create_ogl_warp(ow);

	create_ogl_warp_shaders(&ow->vertex_shaders[0], "shaders/uniform_block.vert",
			&ow->fragment_shaders[0], "shaders/attrib2.frag");

	create_ogl_warp_program(ow->vertex_shaders[0],
			ow->fragment_shaders[0], &ow->programs[0]);

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

	glBindVertexArray(ow->vao[0]);

	glBindBuffer(GL_ARRAY_BUFFER, ow->vbo[0]);
	glBufferData(GL_ARRAY_BUFFER, sizeof(pos_buf), pos_buf, GL_STATIC_DRAW);

	glBindBuffer(GL_ARRAY_BUFFER, ow->vbo[1]);
	glBufferData(GL_ARRAY_BUFFER, sizeof(color_buf), color_buf, GL_STATIC_DRAW);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ow->vbo[2]);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(index_buf), index_buf, GL_STATIC_DRAW);

	// binding attribute
	enum { attr_pos, attr_color };

	glEnableVertexAttribArray(attr_pos);
	glEnableVertexAttribArray(attr_color);

	glVertexAttribFormat(attr_pos, 2, GL_FLOAT, GL_FALSE, 0);
	glVertexAttribFormat(attr_color, 3, GL_FLOAT, GL_FALSE, 0);

	glVertexAttribBinding(attr_pos, 0);
	glVertexAttribBinding(attr_color, 1);

	glBindVertexBuffer(0, ow->vbo[0], 0, 2 * sizeof(GLfloat));
	glBindVertexBuffer(1, ow->vbo[1], 0, 3 * sizeof(GLfloat));

	/* updating uniform block */
	glm::mat4 model_mat = glm::mat4(1.0f);

	glm::mat4 view_mat = glm::lookAt(
			glm::vec3(-2, 1, 2),
			glm::vec3(0, 0, 0),
			glm::vec3(0, 1, 0)
	);

	glm::mat4 proj_mat = glm::perspective(glm::radians(45.0f), 1.0f, 0.1f, 100.0f);

	GLuint ubo_blk_idx = glGetUniformBlockIndex(ow->programs[0], "MVP");
	GLint ubo_sz;
	glGetActiveUniformBlockiv(ow->programs[0], ubo_blk_idx, GL_UNIFORM_BLOCK_DATA_SIZE, &ubo_sz);

	const char *names[3] = {"uModel", "uView", "uProj"};
	GLuint indices[3];
	GLint size[3];
	GLint offset[3];
	GLint type[3];

	glGetUniformIndices(ow->programs[0], 3, names, indices);
	glGetActiveUniformsiv(ow->programs[0], 3, indices, GL_UNIFORM_OFFSET, offset);
	glGetActiveUniformsiv(ow->programs[0], 3, indices, GL_UNIFORM_SIZE, size);
	glGetActiveUniformsiv(ow->programs[0], 3, indices, GL_UNIFORM_TYPE, type);

	/* mat4 here is 4x4=16 GLfloat */
	GLubyte *buffer = (GLubyte *)malloc(ubo_sz);
	memcpy(buffer + offset[0], &model_mat[0][0], size[0] * sizeof(GLfloat) * 16);
	memcpy(buffer + offset[1], &view_mat[0][0], size[1] * sizeof(GLfloat) * 16);
	memcpy(buffer + offset[2], &proj_mat[0][0], size[2] * sizeof(GLfloat) * 16);

	glBindBuffer(GL_UNIFORM_BUFFER, ow->vbo[3]);
	glBufferData(GL_UNIFORM_BUFFER, ubo_sz, buffer, GL_STATIC_DRAW);
	glBindBufferBase(GL_UNIFORM_BUFFER, ubo_blk_idx, ow->vbo[3]);

	while (!glfwWindowShouldClose(ow->win))
	{
		glfwPollEvents();

		glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
		glClear(GL_COLOR_BUFFER_BIT);

		glDrawElements(GL_TRIANGLE_STRIP, 4, GL_UNSIGNED_SHORT, (void*)0);

		glfwSwapBuffers(ow->win);
	}

	glfwTerminate();

	return 0;
}
