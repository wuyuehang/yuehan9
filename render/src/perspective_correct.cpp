#include <SOIL/SOIL.h>
#include <glm/glm.hpp>
#include <glm/gtx/transform.hpp>
#include <glm/gtx/rotate_vector.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include "glrunner.h"

void RenderCB(GlRunner *runner)
{
	glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
	glClearDepth(1.0);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_SHORT, (void*)0);
}

int main()
{
	GlRunner *runner = new GlRunner(RenderCB);

	GLuint vs = runner->BuildShaderProgram("shaders/noperspective.vert", GL_VERTEX_SHADER);
	GLuint fs = runner->BuildShaderProgram("shaders/noperspective.frag", GL_FRAGMENT_SHADER);

	GLuint ppline = runner->BuildProgramPipeline();

	glUseProgramStages(ppline, GL_VERTEX_SHADER_BIT, vs);
	glUseProgramStages(ppline, GL_FRAGMENT_SHADER_BIT, fs);

	GLfloat pos_buf[] = {
			-1.0, -1.0,
			1.0, 1.0,
			-1.0, 1.0,
			1.0, -1.0
	};

	GLfloat texcord_buf[] = {
			0.0, 0.0,
			1.0, 1.0,
			0.0, 1.0,
			1.0, 0.0
	};

	GLushort index_buf[] = {
			0, 1, 2,
			0, 1, 3
	};

	glm::vec3 camera_location(0, 4, 4);

	glm::mat4 view_mat = glm::lookAt(
			camera_location,
			glm::vec3(0, 0, 0),
			glm::vec3(0, 1, 0)
	);

	glm::mat4 proj_mat = glm::perspective(glm::radians(60.0f), 1.0f, 0.01f, 100.0f);

	glm::mat4 mvp_mat = proj_mat * view_mat;

	GLuint mvp_uni = glGetUniformLocation(vs, "uMVP");

	glProgramUniformMatrix4fv(vs, mvp_uni, 1, GL_FALSE, &mvp_mat[0][0]);

	GLuint VAO;
	glGenVertexArrays(1, &VAO);
	glBindVertexArray(VAO);

	GLuint VBO[3];
	glGenBuffers(3, VBO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO[0]);
	glBufferData(GL_ARRAY_BUFFER, sizeof(pos_buf), pos_buf, GL_STATIC_DRAW);

	glBindBuffer(GL_ARRAY_BUFFER, VBO[1]);
	glBufferData(GL_ARRAY_BUFFER, sizeof(texcord_buf), texcord_buf, GL_STATIC_DRAW);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, VBO[2]);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(index_buf), index_buf, GL_STATIC_DRAW);

	enum { attr_pos, attr_texc };

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
	unsigned char *img = SOIL_load_image("materials/checkerboard.jpg", &width, &height, 0, SOIL_LOAD_RGBA);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, img);
	SOIL_free_image_data(img);

	glProgramUniform1i(fs, glGetUniformLocation(fs, "color2D"), 0);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	runner->OnRender();

	return 0;
}
