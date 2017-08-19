#include "glrunner.h"

#define _sz	256
#define _dsa	1

void RenderCB(GlRunner *runner)
{
}

int main()
{
	GlRunner *runner = new GlRunner(RenderCB);

	GLuint datum[_sz];

	for (int i = 0; i < _sz; i++) {
		datum[i] = (GLuint)i;
	}

	GLuint srcBufferObj;
	glGenBuffers(1, &srcBufferObj);
	glBindBuffer(GL_COPY_READ_BUFFER, srcBufferObj);
	glBufferData(GL_COPY_READ_BUFFER, sizeof(datum), datum, GL_DYNAMIC_DRAW);

	GLuint dstBufferObj;
	glGenBuffers(1, &dstBufferObj);
	glBindBuffer(GL_COPY_WRITE_BUFFER, dstBufferObj);
	glBufferData(GL_COPY_WRITE_BUFFER, sizeof(datum), nullptr, GL_DYNAMIC_DRAW);

#if _dsa
	glCopyNamedBufferSubData(srcBufferObj, dstBufferObj, 0, 0, sizeof(datum));
#else
	glCopyBufferSubData(GL_COPY_READ_BUFFER, GL_COPY_WRITE_BUFFER, 0, 0, sizeof(datum));
#endif

	GLint *mapped = (GLint *)glMapBufferRange(GL_COPY_WRITE_BUFFER, 0, _sz, GL_MAP_READ_BIT);

	for (int i = 0; i < _sz; i++) {
		assert(mapped[i] == i);
		//std::cout << "[" << i << "] " << mapped[i] << std::endl;
	}

	GLuint srcTextureObj;
	glGenTextures(1, &srcTextureObj);
	glBindTexture(GL_TEXTURE_2D, srcTextureObj);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA32UI, 8, 8, 0, GL_RGBA_INTEGER, GL_UNSIGNED_INT, datum);

	GLuint pixels[_sz];
#if _dsa
	glGetTextureImage(srcTextureObj, 0, GL_RGBA_INTEGER, GL_UNSIGNED_INT, sizeof(pixels), pixels);
#else
	glGetTexImage(GL_TEXTURE_2D, 0, GL_RGBA_INTEGER, GL_UNSIGNED_INT, pixels);
#endif

	for (int i = 0; i < _sz; i++) {
		assert(pixels[i] == i);
		//std::cout << "[" << i << "] " << pixels[i] << std::endl;
	}
	return 0;
}
