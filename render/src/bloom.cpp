#include <SOIL/SOIL.h>
#include "glrunner.h"
#include <cmath>
#include <ctime>

clock_t start_time;

#define _WEIGHT_NUM 5
GLfloat pWeights[_WEIGHT_NUM] = {0.0};

// srcTexObj---bloomTexObj---vblurTexObj---hblurTexObj---flipTexObj
GLuint srcTexObj;
GLuint bloomTexObj;
GLuint bloomFBO;
GLuint vblurTexObj;
GLuint hblurTexObj;
GLuint flipTexObj;

GLuint mergeFS;

#define _PI 3.1415926
#define _SIGMA 5.0

float GaussianWeight(float sigma, float offset)
{
	return 1.0/sigma/sqrt(2.0*_PI)*exp(-0.5*offset*offset/sigma/sigma);
}

void RenderCB(GlRunner *runner)
{
	glClearColor(0.2, 0.1, 0.2, 0.0);
	glClearDepthf(1.0);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	//runner->DrawQuad(srcTexObj);
	//runner->DrawQuad(bloomTexObj);
	//runner->DrawQuad(vblurTexObj);
	//runner->DrawQuad(hblurTexObj);
	//runner->DrawQuad(flipTexObj);

	clock_t current_time = clock();
	float delta = 0.00001 *(current_time - start_time);

	glProgramUniform1f(mergeFS, glGetUniformLocation(mergeFS, "uTime"), delta);

	glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
}

int main()
{
	start_time = clock();

	// hardcode framebuffer size
	GlRunner *runner = new GlRunner(RenderCB, 1200, 1200);

	/* 1. prepare common resource */
	GLuint VS = runner->BuildShaderProgram("shaders/quad.vert", GL_VERTEX_SHADER);
	GLuint PPO = runner->BuildProgramPipeline();
	glUseProgramStages(PPO, GL_VERTEX_SHADER_BIT, VS);

	GLfloat pos_buf[] = {
		-1.0, 1.0,
		1.0, 1.0,
		-1.0, -1.0,
		1.0, -1.0,
	};

	GLuint posVBO;
	glGenBuffers(1, &posVBO);
	glBindBuffer(GL_ARRAY_BUFFER, posVBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(pos_buf), pos_buf, GL_STATIC_DRAW);

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

	/* 2. bloom render pass, to get brightness texture */
	GLfloat bloom_texc_buf[] = {
		0.0, 1.0,
		1.0, 1.0,
		0.0, 0.0,
		1.0, 0.0,
	};

	GLuint bloomTexcVBO;
	glGenBuffers(1, &bloomTexcVBO);
	glBindBuffer(GL_ARRAY_BUFFER, bloomTexcVBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(bloom_texc_buf), bloom_texc_buf, GL_DYNAMIC_DRAW);

	GLuint bloomVAO;
	glGenVertexArrays(1, &bloomVAO);
	glBindVertexArray(bloomVAO);

	glEnableVertexAttribArray(0);
	glEnableVertexAttribArray(1);

	glVertexAttribFormat(0, 2, GL_FLOAT, GL_FALSE, 0);
	glVertexAttribFormat(1, 2, GL_FLOAT, GL_FALSE, 0);

	glVertexAttribBinding(0, 0);
	glVertexAttribBinding(1, 1);

	glBindVertexBuffer(0, posVBO, 0, 2*sizeof(GLfloat));
	glBindVertexBuffer(1, bloomTexcVBO, 0, 2*sizeof(GLfloat));

	GLuint bloomFS = runner->BuildShaderProgram("shaders/bloom_brightness.frag", GL_FRAGMENT_SHADER);
	glProgramUniform1i(bloomFS, glGetUniformLocation(bloomFS, "tBrightness2D"), 0);
	glUseProgramStages(PPO, GL_FRAGMENT_SHADER_BIT, bloomFS);

	int w, h, c;
	{
		unsigned char *datum = SOIL_load_image("materials/bloom.jpg", &w, &h, &c, SOIL_LOAD_RGBA);

		glGenTextures(1, &srcTexObj);

		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, srcTexObj);

		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, w, h, 0, GL_RGBA, GL_UNSIGNED_BYTE, datum);

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

		SOIL_free_image_data(datum);
	}

	{
		glGenTextures(1, &bloomTexObj);
		glBindTexture(GL_TEXTURE_2D, bloomTexObj);

		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, w, h, 0, GL_RGBA, GL_UNSIGNED_BYTE, nullptr);

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

		glGenFramebuffers(1, &bloomFBO);
		glBindFramebuffer(GL_FRAMEBUFFER, bloomFBO);
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, bloomTexObj, 0);
		assert(GL_FRAMEBUFFER_COMPLETE == glCheckFramebufferStatus(GL_FRAMEBUFFER));

		glClearColor(0.2, 0.1, 0.2, 0.0);
		glClearDepthf(1.0);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		glBindTexture(GL_TEXTURE_2D, srcTexObj);
		glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
	}

	/* gaussian vertical blur */
	GLuint blur_verFS = runner->BuildShaderProgram("shaders/bloom_gaussian_blur_vpass.frag", GL_FRAGMENT_SHADER);
	glUseProgramStages(PPO, GL_FRAGMENT_SHADER_BIT, blur_verFS);
	glProgramUniform1i(blur_verFS, glGetUniformLocation(blur_verFS, "tColor2D"), 0);
	glProgramUniform1fv(blur_verFS, glGetUniformLocation(blur_verFS, "uGaussianWeights"), _WEIGHT_NUM, pWeights);

	{
		glGenTextures(1, &vblurTexObj);
		glBindTexture(GL_TEXTURE_2D, vblurTexObj);

		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, w, h, 0, GL_RGBA, GL_UNSIGNED_BYTE, nullptr);

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

		glBindFramebuffer(GL_FRAMEBUFFER, bloomFBO);
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, vblurTexObj, 0);
		assert(GL_FRAMEBUFFER_COMPLETE == glCheckFramebufferStatus(GL_FRAMEBUFFER));

		glClearColor(0.2, 0.1, 0.2, 0.0);
		glClearDepthf(1.0);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		glBindTexture(GL_TEXTURE_2D, bloomTexObj);
		glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
	}

	/* gaussian horizontal blur */
	GLuint blur_horFS = runner->BuildShaderProgram("shaders/bloom_gaussian_blur_hpass.frag", GL_FRAGMENT_SHADER);
	glUseProgramStages(PPO, GL_FRAGMENT_SHADER_BIT, blur_horFS);
	glProgramUniform1i(blur_horFS, glGetUniformLocation(blur_horFS, "tColor2D"), 0);
	glProgramUniform1fv(blur_horFS, glGetUniformLocation(blur_horFS, "uGaussianWeights"), _WEIGHT_NUM, pWeights);

	{
		glGenTextures(1, &hblurTexObj);
		glBindTexture(GL_TEXTURE_2D, hblurTexObj);

		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, w, h, 0, GL_RGBA, GL_UNSIGNED_BYTE, nullptr);

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

		glBindFramebuffer(GL_FRAMEBUFFER, bloomFBO);
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, hblurTexObj, 0);
		assert(GL_FRAMEBUFFER_COMPLETE == glCheckFramebufferStatus(GL_FRAMEBUFFER));

		glClearColor(0.2, 0.1, 0.2, 0.0);
		glClearDepthf(1.0);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		glBindTexture(GL_TEXTURE_2D, vblurTexObj);
		glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
	}

	/* merge blurred with original texture */
	mergeFS = runner->BuildShaderProgram("shaders/bloom_merge.frag", GL_FRAGMENT_SHADER);
	glUseProgramStages(PPO, GL_FRAGMENT_SHADER_BIT, mergeFS);

	GLfloat flip_texc_buf[] = {
		0.0, 0.0,
		1.0, 0.0,
		0.0, 1.0,
		1.0, 1.0,
	};

	glBindBuffer(GL_ARRAY_BUFFER, bloomTexcVBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(flip_texc_buf), flip_texc_buf, GL_DYNAMIC_DRAW);

	{
		glGenTextures(1, &flipTexObj);
		glBindTexture(GL_TEXTURE_2D, flipTexObj);

		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, w, h, 0, GL_RGBA, GL_UNSIGNED_BYTE, nullptr);

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

		glBindFramebuffer(GL_FRAMEBUFFER, bloomFBO);
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, flipTexObj, 0);
		assert(GL_FRAMEBUFFER_COMPLETE == glCheckFramebufferStatus(GL_FRAMEBUFFER));
	}

	{
		glClearColor(0.2, 0.1, 0.2, 0.0);
		glClearDepthf(1.0);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, hblurTexObj);
		glProgramUniform1i(mergeFS, glGetUniformLocation(mergeFS, "tBlur2D"), 0);

		glActiveTexture(GL_TEXTURE1);
		glBindTexture(GL_TEXTURE_2D, srcTexObj);
		glProgramUniform1i(mergeFS, glGetUniformLocation(mergeFS, "tOrigin2D"), 1);

		glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
	}

	runner->OnRender();

	return 0;
}
