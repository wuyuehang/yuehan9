#include <SOIL/SOIL.h>
#include "glrunner.h"

void RenderCB(GlRunner *runner)
{
	glClearDepthf(1.0);
	glClear(GL_DEPTH_BUFFER_BIT);

	glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
}

int main()
{
	GlRunner *runner = new GlRunner(RenderCB);

	GLuint VS = runner->BuildShaderProgram("shaders/quad.vert", GL_VERTEX_SHADER);
	GLuint FS = runner->BuildShaderProgram("shaders/alpha_map.frag", GL_FRAGMENT_SHADER);
	GLuint PPO = runner->BuildProgramPipeline();

	glUseProgramStages(PPO, GL_VERTEX_SHADER_BIT, VS);
	glUseProgramStages(PPO, GL_FRAGMENT_SHADER_BIT, FS);

	glProgramUniform1i(FS, glGetUniformLocation(FS, "tAlpha2D"), 0);
	glProgramUniform1i(FS, glGetUniformLocation(FS, "tContent2D"), 1);

	GLfloat pos_buf[] = {
		-1.0, 1.0,
		1.0, 1.0,
		-1.0, -1.0,
		1.0, -1.0,
	};

	GLfloat texc_buf[] = {
		0.0, 0.0,
		1.0, 0.0,
		0.0, 1.0,
		1.0, 1.0,
	};

	GLuint VBO[2];
	glGenBuffers(2, VBO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO[0]);
	glBufferData(GL_ARRAY_BUFFER, sizeof(pos_buf), pos_buf, GL_STATIC_DRAW);
	glBindBuffer(GL_ARRAY_BUFFER, VBO[1]);
	glBufferData(GL_ARRAY_BUFFER, sizeof(texc_buf), texc_buf, GL_STATIC_DRAW);

	GLuint VAO;
	glGenVertexArrays(1, &VAO);
	glBindVertexArray(VAO);

	glEnableVertexAttribArray(0);
	glEnableVertexAttribArray(1);

	glVertexAttribFormat(0, 2, GL_FLOAT, GL_FALSE, 0);
	glVertexAttribFormat(1, 2, GL_FLOAT, GL_FALSE, 0);

	glVertexAttribBinding(0, 0);
	glVertexAttribBinding(1, 1);

	glBindVertexBuffer(0, VBO[0], 0, 2*sizeof(GLfloat));
	glBindVertexBuffer(1, VBO[1], 0, 2*sizeof(GLfloat));

	int w, h, c;
	{
		GLuint srcTexObj;
		glGenTextures(1, &srcTexObj);

		unsigned char *pContent = SOIL_load_image("materials/front.jpg", &w, &h, &c, SOIL_LOAD_RGBA);

		glActiveTexture(GL_TEXTURE1);
		glBindTexture(GL_TEXTURE_2D, srcTexObj);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, w, h, 0, GL_RGBA, GL_UNSIGNED_BYTE, pContent);

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

		SOIL_free_image_data(pContent);
	}

	{
		GLuint alphaTexObj;
		glGenTextures(1, &alphaTexObj);

		unsigned char *pAlpha = SOIL_load_image("materials/alpha_map.jpg", &w, &h, &c, SOIL_LOAD_RGBA);

		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, alphaTexObj);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, w, h, 0, GL_RGBA, GL_UNSIGNED_BYTE, pAlpha);

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

		SOIL_free_image_data(pAlpha);
	}

	runner->OnRender();

	return 0;
}
