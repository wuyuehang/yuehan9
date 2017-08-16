#include "glrunner.h"

#define _sz 4

GLuint srcObj;
GLuint writeObj;
GLuint dstObj;

void RenderCB(GlRunner *runner)
{
	glClearColor(0.0, 0.0, 0.0, 0.0);
	glClearDepthf(1.0);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	// draw the final texture
	runner->DrawQuad(dstObj);
}

int main()
{
	GlRunner *runner = new GlRunner(RenderCB);

	GLubyte datum[] = {
		255, 0, 0, 0,
		0, 255, 0, 0,
		0, 0, 255, 0,
		255, 255, 0, 0,
		255, 0, 255, 0,
		0, 255, 255, 0,
		255, 255, 255, 0,
		0, 0, 0, 0,
		125, 0, 0, 0,
		0, 125, 0, 0,
		0, 0, 125, 0,
		125, 125, 0, 0,
		0, 0, 0, 0,
		0, 125, 125, 0,
		125, 0, 125, 0,
		125, 125, 125, 0
	};

	// init normal source texture
	glGenTextures(1, &srcObj);
	glBindTexture(GL_TEXTURE_2D, srcObj);
	glTexStorage2D(GL_TEXTURE_2D, 1, GL_RGBA8, _sz, _sz);
	glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, _sz, _sz, GL_RGBA, GL_UNSIGNED_BYTE, datum);

	// compute the writeObj texture as destination texture
	glGenTextures(1, &writeObj);
	glBindTexture(GL_TEXTURE_2D, writeObj);
	glTexStorage2D(GL_TEXTURE_2D, 1, GL_RGBA8, _sz, _sz);

	glBindImageTexture(0, srcObj, 0, GL_FALSE, 0, GL_READ_ONLY, GL_RGBA8);
	glBindImageTexture(1, writeObj, 0, GL_FALSE, 0, GL_WRITE_ONLY, GL_RGBA8);

	GLuint CS = runner->BuildShaderProgram("shaders/cs_rwbarrier.comp", GL_COMPUTE_SHADER);
	GLuint CSPPO = runner->BuildProgramPipeline();
	glUseProgramStages(CSPPO, GL_COMPUTE_SHADER_BIT, CS);

	glDispatchCompute(1, 1, 1);

	glMemoryBarrier(GL_SHADER_IMAGE_ACCESS_BARRIER_BIT);

	// compute the writeObj texture as source texture
	glGenTextures(1, &dstObj);
	glBindTexture(GL_TEXTURE_2D, dstObj);
	glTexStorage2D(GL_TEXTURE_2D, 1, GL_RGBA8, _sz, _sz);

	glBindImageTexture(0, writeObj, 0, GL_FALSE, 0, GL_READ_ONLY, GL_RGBA8);
	glBindImageTexture(1, dstObj, 0, GL_FALSE, 0, GL_WRITE_ONLY, GL_RGBA8);

	glDispatchCompute(1, 1, 1);

	glMemoryBarrier(GL_SHADER_IMAGE_ACCESS_BARRIER_BIT);

	runner->OnRender();

	return 0;
}
