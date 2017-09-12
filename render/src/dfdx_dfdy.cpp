#include "glrunner.h"

void RenderCB(GlRunner *runner)
{

}

int main()
{
	GlRunner *runner = new GlRunner(RenderCB);

	glViewport(0, 0, 4, 4);
	GLuint VS = runner->BuildShaderProgram("shaders/quad.vert", GL_VERTEX_SHADER);
	GLuint FS = runner->BuildShaderProgram("shaders/dfdx_dfdy.frag", GL_FRAGMENT_SHADER);
	GLuint PPO = runner->BuildProgramPipeline();
	glUseProgramStages(PPO, GL_VERTEX_SHADER_BIT, VS);
	glUseProgramStages(PPO, GL_FRAGMENT_SHADER_BIT, FS);

	GLuint dstTexObj;
	glGenTextures(1, &dstTexObj);
	glBindTexture(GL_TEXTURE_2D, dstTexObj);
	glTexStorage2D(GL_TEXTURE_2D,
			1, // number of level
			GL_R32F, // internal format
			4, // w
			4); // h

	GLuint dstFBO;
	glGenFramebuffers(1, &dstFBO);
	glBindFramebuffer(GL_FRAMEBUFFER, dstFBO);
	glFramebufferTexture2D(GL_FRAMEBUFFER,
			GL_COLOR_ATTACHMENT0,
			GL_TEXTURE_2D,
			dstTexObj,
			0);

	assert(GL_FRAMEBUFFER_COMPLETE == glCheckFramebufferStatus(GL_FRAMEBUFFER));

	GLfloat pos[] = {
		-1.0, 1.0,
		1.0, 1.0,
		-1.0, -1.0,
		1.0, -1.0
	};

	/* represent far object */
	GLfloat uv_0[] = {
		0.0, 1.0,
		1.0, 1.0,
		0.0, 0.0,
		1.0, 0.0
	};

	/* represent near object */
	GLfloat uv_1[] = {
		0.0, 0.25,
		0.25, 0.25,
		0.0, 0.0,
		0.25, 0.0
	};

	GLuint VAO[2], VBO[3];
	glGenVertexArrays(2, VAO);
	glGenBuffers(3, VBO);

	/* draw farther object */
	glBindVertexArray(VAO[0]);

	glBindBuffer(GL_ARRAY_BUFFER, VBO[0]);
	glBufferData(GL_ARRAY_BUFFER, sizeof(pos), pos, GL_STATIC_DRAW);
	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2*sizeof(GLfloat), 0);
	glEnableVertexAttribArray(0);

	glBindBuffer(GL_ARRAY_BUFFER, VBO[1]);
	glBufferData(GL_ARRAY_BUFFER, sizeof(uv_0), uv_0, GL_STATIC_DRAW);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 2*sizeof(GLfloat), 0);
	glEnableVertexAttribArray(1);

	glClearColor(0.0, 0.0, 0.0, 0.0);
	glClearDepthf(1.0);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);

	GLfloat res_0[16];
	glReadPixels(0, 0, 4, 4, GL_RED, GL_FLOAT, res_0);

	std::cout << "\n######## far object ########" << std::endl;
	for (int i = 3; i >= 0; i--) {
		for (int j = 0; j < 4; j++) {
			std::cout << res_0[i*4+j] << ", ";
		}
		std::cout << std::endl;
	}

	/* draw near object */
	glBindVertexArray(VAO[1]);

	glBindBuffer(GL_ARRAY_BUFFER, VBO[0]);
	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2*sizeof(GLfloat), 0);
	glEnableVertexAttribArray(0);

	glBindBuffer(GL_ARRAY_BUFFER, VBO[2]);
	glBufferData(GL_ARRAY_BUFFER, sizeof(uv_1), uv_1, GL_STATIC_DRAW);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 2*sizeof(GLfloat), 0);
	glEnableVertexAttribArray(1);

	glClearColor(0.0, 0.0, 0.0, 0.0);
	glClearDepthf(1.0);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);

	GLfloat res_1[16];
	glReadPixels(0, 0, 4, 4, GL_RED, GL_FLOAT, res_1);

	std::cout << "\n######## near object ########" << std::endl;
	for (int i = 3; i >= 0; i--) {
		for (int j = 0; j < 4; j++) {
			std::cout << res_1[i*4+j] << ", ";
		}
		std::cout << std::endl;
	}

	return 0;
}
