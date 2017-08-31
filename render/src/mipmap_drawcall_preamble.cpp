#include "glrunner.h"

void RenderCB(GlRunner *runner)
{

}

// drawcall style to generate mipmap for POT texture
// here draw a 4x4 mip0 to 2x2 mip1 as an example
int main()
{
	GlRunner *runner = new GlRunner(RenderCB, 2, 2);

	//glPixelStorei(GL_PACK_ALIGNMENT, 1);
	//glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

	GLubyte srcData[] = {
		255, 0, 0, 0,
		0, 255, 0, 0,
		128, 0, 0, 0,
		0, 128, 0, 0,
		0, 0, 255, 0,
		0, 0, 0, 255,
		0, 0, 128, 0,
		0, 0, 0, 128,
		64, 0, 0, 0,
		0, 64, 0, 0,
		32, 0, 0, 0,
		0, 32, 0, 0,
		0, 0, 64, 0,
		0, 0, 0, 64,
		0, 0, 32, 0,
		0, 0, 0, 32,
	};

	std::cout << "######## original data #######" << std::endl;
	for (int i = 0; i < 4; i++) {
		for (int j = 0; j < 16; j++) {
			if (j%4 == 0) {
				std::cout << "(";
			}
			std::cout << (int)srcData[i*16+j];
			if (j%4 == 3) {
				std::cout << ")";
			} else {
				std::cout << ", ";
			}
		}
		std::cout << std::endl;
	}

	GLuint srcTexObj;
	glGenTextures(1, &srcTexObj);
	glBindTexture(GL_TEXTURE_2D, srcTexObj);
	glTexImage2D(GL_TEXTURE_2D,
			0, // base level
			GL_RGBA8, // internal format
			4, // w
			4, // h
			0, // border
			GL_RGBA, // format
			GL_UNSIGNED_BYTE, // type
			srcData);

	GLuint VS = runner->BuildShaderProgram("shaders/quad.vert", GL_VERTEX_SHADER);
	GLuint FS = runner->BuildShaderProgram("shaders/quad.frag", GL_FRAGMENT_SHADER);
	GLuint PPO = runner->BuildProgramPipeline();
	glUseProgramStages(PPO, GL_VERTEX_SHADER_BIT, VS);
	glUseProgramStages(PPO, GL_FRAGMENT_SHADER_BIT, FS);

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, srcTexObj);
	glProgramUniform1i(FS, glGetUniformLocation(FS, "color2D"), 0);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	// vertex data preparation
	GLfloat pos_buf[] = {
		-1., -1.,
		-1., 1.,
		1., 1.,
		//
		-1., -1.,
		1., 1.,
		1., -1.
	};

	GLfloat texc_buf[] = {
		0., 0.,
		0., 1.,
		1., 1.,
		//
		0., 0.,
		1., 1.,
		1., 0.
	};

	GLuint mipmapVAO;
	glGenVertexArrays(1, &mipmapVAO);
	glBindVertexArray(mipmapVAO);

	GLuint VBO[2];
	glGenBuffers(2, VBO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO[0]);
	glBufferData(GL_ARRAY_BUFFER, sizeof(pos_buf), pos_buf, GL_STATIC_DRAW);
	glBindBuffer(GL_ARRAY_BUFFER, VBO[1]);
	glBufferData(GL_ARRAY_BUFFER, sizeof(texc_buf), texc_buf, GL_STATIC_DRAW);

	enum {attr_pos, attr_texc};
	glEnableVertexAttribArray(attr_pos);
	glVertexAttribFormat(attr_pos, 2, GL_FLOAT, GL_FALSE, 0);
	glVertexAttribBinding(attr_pos, 0);
	glBindVertexBuffer(attr_pos, VBO[0], 0, 2*sizeof(GLfloat));

	glEnableVertexAttribArray(attr_texc);
	glVertexAttribFormat(attr_texc, 2, GL_FLOAT, GL_FALSE, 0);
	glVertexAttribBinding(attr_texc, 1);
	glBindVertexBuffer(attr_texc, VBO[1], 0, 2*sizeof(GLfloat));

	glClearColor(0.0, 0.0, 0.0, 0.0);
	glClearDepthf(1.0);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glDrawArrays(GL_TRIANGLES, 0, 6);

	glFinish();

	// glReadPixels isn't related with double buffer swap
	glfwSwapBuffers(runner->win());

	GLubyte res[16];
	glReadPixels(0, 0, 2, 2, GL_RGBA, GL_UNSIGNED_BYTE, res);

	std::cout << "######## result data #######" << std::endl;

	for (int i = 0; i < 2; i++) {
		for (int j = 0; j < 8; j++) {
			if (j%4 == 0) {
				std::cout << "(";
			}
			std::cout << (int)res[i*8+j];
			if (j%4 == 3) {
				std::cout << ")";
			} else {
				std::cout << ", ";
			}
		}
		std::cout << std::endl;
	}

	return 0;
}
