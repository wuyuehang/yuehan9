#include <SOIL/SOIL.h>
#include "ogl_warp.h"

#define _enable_cull_face_ 1
int main(int argc, char **argv)
{
	struct ogl_warp *ow = (struct ogl_warp *)malloc(sizeof(struct ogl_warp));
	memset(ow, 0, sizeof(*ow));

	create_ogl_warp(ow);

	create_ogl_warp_shaders(&ow->vertex_shaders[0], "shaders/quad.vert",
			&ow->fragment_shaders[0], "shaders/quad.frag");

	create_ogl_warp_program(ow->vertex_shaders[0],
			ow->fragment_shaders[0], &ow->programs[0]);

#if _enable_cull_face_
	glEnable(GL_CULL_FACE);
	glCullFace(GL_BACK);
	glFrontFace(GL_CW);
#endif

	GLfloat pos_buf[] = {
			-0.25f, 0.25f,
			0.25f, -0.25f,
			-0.25f, -0.25f,
			0.25f, 0.25f
	};

	GLfloat texcord_buf[] = {
			0.0f, 0.0f,
			1.0f, 1.0f,
			0.0f, 1.0f,
			1.0f, 0.0f
	};

	GLushort index_buf[] = {
			0, 1, 2,
			0, 1, 3
	};

	glBindVertexArray(ow->vao[0]);

	glBindBuffer(GL_ARRAY_BUFFER, ow->vbo[0]);
	glBufferData(GL_ARRAY_BUFFER, sizeof(pos_buf), pos_buf, GL_STATIC_DRAW);

	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(GLfloat), (GLvoid *)0);

	glBindBuffer(GL_ARRAY_BUFFER, ow->vbo[1]);
	glBufferData(GL_ARRAY_BUFFER, sizeof(texcord_buf), texcord_buf, GL_STATIC_DRAW);

	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(GLfloat), (GLvoid *)0);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ow->vbo[2]);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(index_buf), index_buf, GL_STATIC_DRAW);

	/* loading texture image */
	GLuint textures[1];
	glGenTextures(1, textures);

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, textures[0]);
	int width, height;
	unsigned char *img = SOIL_load_image("materials/v4v.jpg", &width, &height, 0, SOIL_LOAD_RGBA);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, img);
	SOIL_free_image_data(img);

	glUniform1i(glGetUniformLocation(ow->programs[0], "color2D"), 0);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	while (!glfwWindowShouldClose(ow->win))
	{
		glfwPollEvents();

		glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
		glClear(GL_COLOR_BUFFER_BIT);

		glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_SHORT, (void*)0);

		glfwSwapBuffers(ow->win);
	}

	glfwTerminate();

	return 0;
}
