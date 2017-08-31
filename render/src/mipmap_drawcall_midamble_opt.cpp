#include "glrunner.h"

void RenderCB(GlRunner *runner)
{

}

int main()
{
	GlRunner *runner = new GlRunner(RenderCB);

	// preapare source texture
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
		0, 0, 0, 32
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
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	// prepare target texture
	GLuint dstColorTexObj;
	glGenTextures(1, &dstColorTexObj);
	glBindTexture(GL_TEXTURE_2D, dstColorTexObj);
	glTexImage2D(GL_TEXTURE_2D,
			0, // base level
			GL_RGBA8, // internal format
			2, // w
			2, // h
			0, // border
			GL_RGBA, // format
			GL_UNSIGNED_BYTE, // type
			nullptr);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	// prepare vertex data
	GLfloat pos_buf[] = {
		-1., -1.,
		-1., 1.,
		1., 1.,
		-1., -1.,
		1., 1.,
		1., -1.
	};

	GLfloat texc_buf[] = {
		0., 0.,
		0., 1.,
		1., 1.,
		0., 0.,
		1., 1.,
		1., 0.
	};

	GLuint VBO[2];
	GLuint VAO;
	glGenVertexArrays(1, &VAO);
	glBindVertexArray(VAO);

	glGenBuffers(2, VBO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO[0]);
	glBufferData(GL_ARRAY_BUFFER, sizeof(pos_buf), pos_buf, GL_STATIC_DRAW);
	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2*sizeof(GLfloat), (GLvoid*)0);
	glEnableVertexAttribArray(0);

	glBindBuffer(GL_ARRAY_BUFFER, VBO[1]);
	glBufferData(GL_ARRAY_BUFFER, sizeof(texc_buf), texc_buf, GL_STATIC_DRAW);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 2*sizeof(GLfloat), (GLvoid*)0);
	glEnableVertexAttribArray(1);

	// prepare program
	GLuint VS = runner->BuildShaderProgram("shaders/quad.vert", GL_VERTEX_SHADER);
	GLuint FS = runner->BuildShaderProgram("shaders/pot_quad.frag", GL_FRAGMENT_SHADER);
	GLuint PPO = runner->BuildProgramPipeline();
	glUseProgramStages(PPO, GL_VERTEX_SHADER_BIT, VS);
	glUseProgramStages(PPO, GL_FRAGMENT_SHADER_BIT, FS);

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, srcTexObj);
	glProgramUniform1i(FS, glGetUniformLocation(FS, "color2D"), 0);

	GLuint FBO;
	glGenFramebuffers(1, &FBO);
	glBindFramebuffer(GL_FRAMEBUFFER, FBO);

	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, dstColorTexObj, 0);
	assert(GL_FRAMEBUFFER_COMPLETE == glCheckFramebufferStatus(GL_FRAMEBUFFER));

	glBindFramebuffer(GL_DRAW_FRAMEBUFFER, FBO);

	// reset the right viewport
	// from 4x4 to 2x2
	glViewport(0, 0, 2, 2);
	glClearColor(0.0, 0.0, 0.0, 0.0);
	glClear(GL_COLOR_BUFFER_BIT);

	glDrawArrays(GL_TRIANGLES, 0, 6);

	glFinish();

	// verify from framebuffer data on FBO
	GLubyte res[16];
	glReadPixels(0, 0, 2, 2, GL_RGBA, GL_UNSIGNED_BYTE, res);

	std::cout << "######## framebuffer pixel data #######" << std::endl;

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

	// verify from texture image data
	glBindTexture(GL_TEXTURE_2D, dstColorTexObj);
	GLubyte res2[16];

	glGetTexImage(GL_TEXTURE_2D,
			0, // base level
			GL_RGBA, // format
			GL_UNSIGNED_BYTE, // type
			res2);

	std::cout << "######## target texture image data ########" << std::endl;
	for (int i = 0; i < 2; i++) {
		for (int j = 0; j < 8; j++) {
			if (j%4 == 0) {
				std::cout << "(";
			}
			std::cout << (int)res2[i*8+j];
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
