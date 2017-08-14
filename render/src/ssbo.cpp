#include "glrunner.h"

#define sz	256

void RenderCB(GlRunner *runner)
{
	glClearColor(0.0, 0.0, 0.0, 0.0);
	glClearDepthf(1.0);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

int main()
{
	GlRunner *runner = new GlRunner(RenderCB);

	// generate texture buffer holds texel
	GLint data[sz];

	for (int i = 0; i < sz; i++) {
		data[i] = (GLint)i;
	}

	GLuint tb;

#if 0
	glGenBuffers(1, &tb);
	glBindBuffer(GL_TEXTURE_BUFFER, tb);
	glBufferData(GL_TEXTURE_BUFFER, sizeof(data), data, GL_DYNAMIC_DRAW);
#else
	// immutable
	glCreateBuffers(1, &tb);
	glNamedBufferStorage(tb, sizeof(data), data, GL_DYNAMIC_STORAGE_BIT);
#endif

	// bind texture buffer to a texture object
	GLuint txc;
	glGenTextures(1, &txc);
	glBindTexture(GL_TEXTURE_BUFFER, txc);
	glTexBuffer(GL_TEXTURE_BUFFER, GL_RGBA32I, tb);
	glBindImageTexture(0, txc, 0, GL_FALSE, 0, GL_READ_ONLY, GL_RGBA32I);

	// set up ssbo for result
	GLuint ssbo;
	glGenBuffers(1, &ssbo);
	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 0, ssbo);
	glBufferData(GL_SHADER_STORAGE_BUFFER, sizeof(data), nullptr, GL_DYNAMIC_DRAW);

	// compute shader
	GLuint CS = runner->BuildShaderProgram("shaders/ssbo.comp", GL_COMPUTE_SHADER);
	GLuint cspipe = runner->BuildProgramPipeline();

	glUseProgramStages(cspipe, GL_COMPUTE_SHADER_BIT, CS);

	glDispatchCompute(1, 1, 1);

	glMemoryBarrier(GL_BUFFER_UPDATE_BARRIER_BIT);

#if 0
	GLint *mapped = (GLint *)glMapBufferRange(GL_SHADER_STORAGE_BUFFER, 0, sz, GL_MAP_READ_BIT);
#else
	GLint *mapped = (GLint *)glMapNamedBufferRange(ssbo, 0, sz, GL_MAP_READ_BIT);
#endif

	for (int i = 0; i < sz; i++) {
		assert((GLuint)i == mapped[i]);
		//std::cout << "[" << i << "] " << mapped[i] << std::endl;
	}

#if 0
	glUnmapBuffer(GL_SHADER_STORAGE_BUFFER);
#else
	glUnmapNamedBuffer(ssbo);
#endif

	return 0;
}
