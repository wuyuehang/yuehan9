#include "glrunner.h"

void RenderCB(GlRunner *runner)
{

}

int main()
{
	GlRunner *runner = new GlRunner(RenderCB);

	// 1. initial source client data
	// choose single component with float because of
	// automatic mipmap generation limitation on intel
	// integrated card.
	GLfloat clientDatum[256];
	for (int i = 0; i < 256; i++) {
		clientDatum[i] = (GLfloat)i;
	}

	for (int i = 0; i < 16; i++) {
		for (int j = 0; j < 16; j++) {
			std::cout << clientDatum[16*i+j] << ", ";
		}
		std::cout << std::endl;
	}

	// 2. initial source texture object
	// using source client data
	GLuint srcTextureObj;
	glGenTextures(1, &srcTextureObj);
	glBindTexture(GL_TEXTURE_2D, srcTextureObj);
	glTexImage2D(GL_TEXTURE_2D,
			0, // base mipmap level
			GL_R32F, // internal format
			16, // width
			16, // height
			0, // border
			GL_RED, // format
			GL_FLOAT, // type
			clientDatum);

#if _dsa
	glGenerateTextureMipmap(srcTextureObj);
#else
	glGenerateMipmap(GL_TEXTURE_2D);
#endif

	GLfloat resultDatum[256];
	for (int mip = 1; mip <= 4;mip++) {

		glGetTexImage(GL_TEXTURE_2D,
				mip, // base mipmap level
				GL_RED, // format
				GL_FLOAT, // type
				resultDatum);

		std::cout << "###### mipmap " << mip << " ######" << std::endl;
		for (int i = 0; i < (16 >> mip); i++) {
			for (int j = 0; j < (16 >> mip); j++) {
				std::cout << resultDatum[i*(16 >> mip)+j] << ", ";
			}
			std::cout << std::endl;
		}
	}

	return 0;
}
