#include <SOIL/SOIL.h>
#include "glrunner.h"

GLuint srcObj;
GLuint writeObj;
GLuint dstObj;

#define _sz 1024

void RenderCB(GlRunner *runner)
{
	glClearColor(0.0, 0.0, 0.0, 0.0);
	glClearDepthf(1.0);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	// draw the final texture
	// double flip negative operation on readonly image will
	// not differ from original image, so draw the writeObj
	// as quad instead.
	runner->DrawQuad(writeObj);
}

int main()
{
	GlRunner *runner = new GlRunner(RenderCB);

	int w, h, c;
	unsigned char *datum = SOIL_load_image("materials/stormtrooper.jpg", &w, &h, &c, SOIL_LOAD_RGBA);

	// init normal source texture
	glGenTextures(1, &srcObj);
	glBindTexture(GL_TEXTURE_2D, srcObj);
	glTexStorage2D(GL_TEXTURE_2D, 1, GL_RGBA8, _sz, _sz);
	glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, _sz, _sz, GL_RGBA, GL_UNSIGNED_BYTE, datum);

	SOIL_free_image_data(datum);

	// compute the writeObj texture as destination texture
	glGenTextures(1, &writeObj);
	glBindTexture(GL_TEXTURE_2D, writeObj);
	glTexStorage2D(GL_TEXTURE_2D, 1, GL_RGBA8, _sz, _sz);

	glBindImageTexture(0, srcObj, 0, GL_FALSE, 0, GL_READ_ONLY, GL_RGBA8);
	glBindImageTexture(1, writeObj, 0, GL_FALSE, 0, GL_WRITE_ONLY, GL_RGBA8);

	GLuint CS = runner->BuildShaderProgram("shaders/compute_large_image.comp", GL_COMPUTE_SHADER);
	GLuint CSPPO = runner->BuildProgramPipeline();
	glUseProgramStages(CSPPO, GL_COMPUTE_SHADER_BIT, CS);

	glDispatchCompute(1, _sz, 1);

	glMemoryBarrier(GL_SHADER_IMAGE_ACCESS_BARRIER_BIT);

	// compute the writeObj texture as source texture
	glGenTextures(1, &dstObj);
	glBindTexture(GL_TEXTURE_2D, dstObj);
	glTexStorage2D(GL_TEXTURE_2D, 1, GL_RGBA8, _sz, _sz);

	glBindImageTexture(0, writeObj, 0, GL_FALSE, 0, GL_READ_ONLY, GL_RGBA8);
	glBindImageTexture(1, dstObj, 0, GL_FALSE, 0, GL_WRITE_ONLY, GL_RGBA8);

	glDispatchCompute(1, _sz, 1);

	glMemoryBarrier(GL_SHADER_IMAGE_ACCESS_BARRIER_BIT);

	runner->OnRender();

	return 0;
}
