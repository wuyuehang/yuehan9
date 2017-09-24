#include <SOIL/SOIL.h>
#include "glrunner.h"
#include <cmath>

GLuint PPO;
GLuint FS_blur_ver;
GLuint FS_blur_hor;

#define _WEIGHT_NUM 5
GLfloat pWeights[_WEIGHT_NUM] = {0.0};

GLuint srcTexObj;
GLuint tempTexObj;
GLuint tempFBO;

#define _PI 3.1415926
#define _SIGMA 5.0

float GaussianWeight(float sigma, float offset)
{
	return 1.0/sigma/sqrt(2.0*_PI)*exp(-0.5*offset*offset/sigma/sigma);
}

void RenderCB(GlRunner *runner)
{
	double xpos, ypos;
	glfwGetCursorPos(runner->win(), &xpos, &ypos);
	xpos = std::max(0.0, std::min(1.0, xpos / GR_WIDTH));
	ypos = std::max(0.0, std::min(1.0, ypos / GR_HEIGHT));

	// gaussian blur vertical render pass (!!render to texture)
	glBindFramebuffer(GL_FRAMEBUFFER, tempFBO);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, tempTexObj, 0);
	assert(GL_FRAMEBUFFER_COMPLETE == glCheckFramebufferStatus(GL_FRAMEBUFFER));

	glClearColor(0.2, 0.1, 0.2, 0.0);
	glClearDepthf(1.0);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glBindTexture(GL_TEXTURE_2D, srcTexObj);
	glUseProgramStages(PPO, GL_FRAGMENT_SHADER_BIT, FS_blur_ver);
	glProgramUniform2f(FS_blur_ver, glGetUniformLocation(FS_blur_ver, "uMouse"), float(xpos), float(ypos));
	glProgramUniform1i(FS_blur_ver, glGetUniformLocation(FS_blur_ver, "tColor2D"), 0);
	glProgramUniform1fv(FS_blur_ver, glGetUniformLocation(FS_blur_ver, "uGaussianWeights"), _WEIGHT_NUM, pWeights);
	glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);

	// gaussian blur horizontal render pass
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glClearColor(0.2, 0.1, 0.2, 0.0);
	glClearDepthf(1.0);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glBindTexture(GL_TEXTURE_2D, tempTexObj);
	glUseProgramStages(PPO, GL_FRAGMENT_SHADER_BIT, FS_blur_hor);
	glProgramUniform2f(FS_blur_hor, glGetUniformLocation(FS_blur_hor, "uMouse"), float(xpos), float(ypos));
	glProgramUniform1i(FS_blur_hor, glGetUniformLocation(FS_blur_hor, "tColor2D"), 0);
	glProgramUniform1fv(FS_blur_hor, glGetUniformLocation(FS_blur_hor, "uGaussianWeights"), _WEIGHT_NUM, pWeights);
	glDrawArrays(GL_TRIANGLE_STRIP, 4, 4);
}

int main()
{
	GlRunner *runner = new GlRunner(RenderCB);

	GLuint VS = runner->BuildShaderProgram("shaders/quad.vert", GL_VERTEX_SHADER);
	FS_blur_ver = runner->BuildShaderProgram("shaders/gaussian_blur_vpass.frag", GL_FRAGMENT_SHADER);
	FS_blur_hor = runner->BuildShaderProgram("shaders/gaussian_blur_hpass.frag", GL_FRAGMENT_SHADER);
	PPO = runner->BuildProgramPipeline();

	glUseProgramStages(PPO, GL_VERTEX_SHADER_BIT, VS);

	GLfloat pos_buf[] = {
		// vertical renderpass
		-1.0, 1.0,
		1.0, 1.0,
		-1.0, -1.0,
		1.0, -1.0,
		// horizontal renderpass
		-1.0, 1.0,
		1.0, 1.0,
		-1.0, -1.0,
		1.0, -1.0
	};

	GLfloat texc_buf[] = {
		// vertical renderpass
		0.0, 0.0,
		1.0, 0.0,
		0.0, 1.0,
		1.0, 1.0,
		// horizontal renderpass
		0.0, 1.0,
		1.0, 1.0,
		0.0, 0.0,
		1.0, 0.0
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
	unsigned char *datum = SOIL_load_image("materials/stormtrooper.jpg", &w, &h, &c, SOIL_LOAD_RGBA);

	glGenTextures(1, &srcTexObj);

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, srcTexObj);

	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, w, h, 0, GL_RGBA, GL_UNSIGNED_BYTE, datum);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

	SOIL_free_image_data(datum);

	/* generate gaussian weight */
	float sum = 0.0;

	for (int i = -4; i < 0; i++) {
		pWeights[i+4] = GaussianWeight(_SIGMA, (0-i)/4.0);
		sum += pWeights[i+4];
	}

	sum *= 2.0;

	pWeights[_WEIGHT_NUM-1] = GaussianWeight(_SIGMA, 0.0);
	sum += pWeights[_WEIGHT_NUM-1];

	for (int i = 0; i < _WEIGHT_NUM; i++) {
		pWeights[i] /= sum;
		std::cout << pWeights[i] << std::endl;
	}

	/* generate temp texture object for storing vertical gaussian pass result */
	glGenTextures(1, &tempTexObj);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, tempTexObj);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, GR_WIDTH, GR_HEIGHT, 0, GL_RGBA, GL_UNSIGNED_BYTE, nullptr);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

	glGenFramebuffers(1, &tempFBO);

	runner->OnRender();

	return 0;
}
