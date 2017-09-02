#include "glrunner.h"

void RenderCB(GlRunner *runner)
{

}

int main()
{
	GlRunner *runner = new GlRunner(RenderCB);

	// prepare base level data
	GLfloat srcData[64];

	std::cout << "######## original data ########" << std::endl;
	for (int i = 0; i < 8; i++) {
		for (int j = 0; j < 8; j++) {
			srcData[i*8+j] = i*8+j;
			std::cout << srcData[i*8+j] << ", ";
		}
		std::cout << std::endl;
	}

	GLuint srcTexObj;
	glGenTextures(1, &srcTexObj);
	glBindTexture(GL_TEXTURE_2D, srcTexObj);
	glTexImage2D(GL_TEXTURE_2D,
			0, // base level
			GL_R32F, // internal format
			8, // w
			8, // h
			0, // border
			GL_RED, // format
			GL_FLOAT, // type
			srcData);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	// prepare next level texture
	GLuint dstTexObj;
	glGenTextures(1, &dstTexObj);
	glBindTexture(GL_TEXTURE_2D, dstTexObj);
	glTexImage2D(GL_TEXTURE_2D,
			0, // base level
			GL_R32F, // internal format
			4, // w
			4, // h
			0, // border
			GL_RED, // format
			GL_FLOAT, // type
			nullptr);

	// lock and load
	glBindImageTexture(
			0, // unit
			srcTexObj, // texture
			0, // level
			GL_FALSE, // layered
			0, // layer
			GL_READ_ONLY, // access
			GL_R32F);

	glBindImageTexture(
			1, // unit
			dstTexObj, // texture
			0, // level
			GL_FALSE, // layered
			0, // layer
			GL_WRITE_ONLY, // access
			GL_R32F);

	GLuint CS = runner->BuildShaderProgram("shaders/mipmap_cs_preamble.comp", GL_COMPUTE_SHADER);
	GLuint PPO = runner->BuildProgramPipeline();
	glUseProgramStages(PPO, GL_COMPUTE_SHADER_BIT, CS);

	glDispatchCompute(1, 1, 1);

	glMemoryBarrier(GL_SHADER_IMAGE_ACCESS_BARRIER_BIT);

	// verify target texture image data
	GLfloat res[16];

	glBindTexture(GL_TEXTURE_2D, dstTexObj);
	glGetTexImage(GL_TEXTURE_2D,
			0, // base level
			GL_RED, // format
			GL_FLOAT, // type
			res);

	std::cout << "######## dst texture image data ########" << std::endl;

	for (int i = 0; i < 4; i++) {
		for (int j = 0; j < 4; j++) {
			std::cout << res[i*4+j] << ", ";
		}
		std::cout << std::endl;
	}

	return 0;
}
