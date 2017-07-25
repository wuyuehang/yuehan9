#include <SOIL/SOIL.h>
#include "glrunner.h"

void RenderCB(GlRunner *runner)
{
	glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
	glClearDepth(1.0);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_SHORT, (void*)0);
}

#define _enable_cull_face_ 1
int main(int argc, char **argv)
{
	GlRunner *runner = new GlRunner(RenderCB);

	GLuint vs = runner->BuildShaderProgram("shaders/quad.vert", GL_VERTEX_SHADER);
	GLuint fs = runner->BuildShaderProgram("shaders/quad.frag", GL_FRAGMENT_SHADER);

	GLuint pl = runner->BuildProgramPipeline();

	glUseProgramStages(pl, GL_VERTEX_SHADER_BIT, vs);
	glUseProgramStages(pl, GL_FRAGMENT_SHADER_BIT, fs);

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

	GLuint VAO, VBO[3];
	glGenVertexArrays(1, &VAO);
	glBindVertexArray(VAO);

	glGenBuffers(3, VBO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO[0]);
	glBufferData(GL_ARRAY_BUFFER, sizeof(pos_buf), pos_buf, GL_STATIC_DRAW);
	glBindBuffer(GL_ARRAY_BUFFER, VBO[1]);
	glBufferData(GL_ARRAY_BUFFER, sizeof(texcord_buf), texcord_buf, GL_STATIC_DRAW);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, VBO[2]);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(index_buf), index_buf, GL_STATIC_DRAW);

	enum {attr_pos, attr_texc};
	glEnableVertexAttribArray(attr_pos);
	glVertexAttribFormat(attr_pos, 2, GL_FLOAT, GL_FALSE, 0);
	glVertexAttribBinding(attr_pos, 0);
	glBindVertexBuffer(attr_pos, VBO[0], 0, 2*sizeof(GLfloat));

	glEnableVertexAttribArray(attr_texc);
	glVertexAttribFormat(attr_texc, 2, GL_FLOAT, GL_FALSE, 0);
	glVertexAttribBinding(attr_texc, 1);
	glBindVertexBuffer(attr_texc, VBO[1], 0, 2*sizeof(GLfloat));

	/* loading texture image */
	GLuint textures[1];
	glGenTextures(1, textures);

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, textures[0]);
	int width, height;
	unsigned char *img = SOIL_load_image("materials/v4v.jpg", &width, &height, 0, SOIL_LOAD_RGBA);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, img);
	SOIL_free_image_data(img);

	glProgramUniform1i(fs, glGetUniformLocation(fs, "color2D"), 0);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	runner->OnRender();

	return 0;
}
