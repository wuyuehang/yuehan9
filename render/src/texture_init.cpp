#include "glrunner.h"

GLuint texobj;

void RenderCB(GlRunner *runner)
{
	glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
	glClearDepth(1.0);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	runner->DrawQuad(texobj);
}

int main()
{
	GlRunner *runner = new GlRunner(RenderCB);

	GLubyte datum[] = {
		255, 0, 0, 0,
		0, 255, 0, 0,
		0, 0, 255, 0,
		255, 255, 0, 0
	};

	glGenTextures(1, &texobj);
	glBindTexture(GL_TEXTURE_2D, texobj);

	// diverge happens, when initialize a texture will storage, it's marked as an immutable object.
	// thus, follow a TexImage2D is forbidden while TexSubImage2D is accepted.
#if 0
	glTexStorage2D(GL_TEXTURE_2D, 1, GL_RGBA8, 2, 2);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, 2, 2, 0, GL_RGBA, GL_UNSIGNED_BYTE, datum);
#else
	glTexStorage2D(GL_TEXTURE_2D, 1, GL_RGBA8, 2, 2);
	glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, 2, 2, GL_RGBA, GL_UNSIGNED_BYTE, datum);
#endif

	runner->OnRender();

	return 0;
}
