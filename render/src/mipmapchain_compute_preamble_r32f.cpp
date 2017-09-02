#include "glrunner.h"

#define _mipmap_w	16
#define _mipmap_h	16
#define _num_mipmap_chain	4

void RenderCB(GlRunner *runner)
{

}

int main()
{
	GlRunner *runner = new GlRunner(RenderCB);

	// prepare base level data
	GLfloat srcData[_mipmap_w*_mipmap_h];

	std::cout << "######## original data ########" << std::endl;
	for (int i = 0; i < _mipmap_w; i++) {
		for (int j = 0; j < _mipmap_h; j++) {
			srcData[i*_mipmap_w+j] = i*_mipmap_w+j;
			std::cout << srcData[i*_mipmap_w+j] << ", ";
		}
		std::cout << std::endl;
	}

	GLuint srcTexObj[5];
	glGenTextures(5, srcTexObj);
	int w = _mipmap_w;
	int h = _mipmap_h;

	for (int i = 0; i <= _num_mipmap_chain; i++) {

		glBindTexture(GL_TEXTURE_2D, srcTexObj[i]);

		if (i == 0) {
			glTexImage2D(GL_TEXTURE_2D,
					0, // base level
					GL_R32F, // internal format
					w, // w
					h, // h
					0, // border
					GL_RED, // format
					GL_FLOAT, // type
					srcData);
		} else {
			glTexImage2D(GL_TEXTURE_2D,
					0, // base level
					GL_R32F, // internal format
					w, // w
					h, // h
					0, // border
					GL_RED, // format
					GL_FLOAT, // type
					srcData);
		}

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

		w = std::max(1, w >> 1);
		h = std::max(1, h >> 1);
	}

	w = _mipmap_w;
	h = _mipmap_h;

	GLuint CS = runner->BuildShaderProgram("shaders/mipmap_cs_preamble.comp", GL_COMPUTE_SHADER);
	GLuint PPO = runner->BuildProgramPipeline();
	glUseProgramStages(PPO, GL_COMPUTE_SHADER_BIT, CS);

	// lock and load
	for (int k = 0; k < _num_mipmap_chain; k++) {

		w = std::max(1, w >> 1);
		h = std::max(1, h >> 1);

		glBindImageTexture(
				0, // unit
				srcTexObj[k], // texture
				0, // level
				GL_FALSE, // layered
				0, // layer
				GL_READ_ONLY, // access
				GL_R32F);

		glBindImageTexture(
				1, // unit
				srcTexObj[k+1], // texture
				0, // level
				GL_FALSE, // layered
				0, // layer
				GL_WRITE_ONLY, // access
				GL_R32F);

		glDispatchCompute(w, w, 1);

		glMemoryBarrier(GL_SHADER_IMAGE_ACCESS_BARRIER_BIT);

		// verify target texture image data
		GLfloat res[w*h];

		glBindTexture(GL_TEXTURE_2D, srcTexObj[k+1]);
		glGetTexImage(GL_TEXTURE_2D,
				0, // base level
				GL_RED, // format
				GL_FLOAT, // type
				res);

		std::cout << "######## target texture image data level(" << k+1 << ") ########" << std::endl;

		for (int i = 0; i < w; i++) {
			for (int j = 0; j < h; j++) {
				std::cout << res[i*w+j] << ", ";
			}
			std::cout << std::endl;
		}
	}

	return 0;
}
