#include "glrunner.h"
#include <cstring>

void RenderCB(GlRunner *runner)
{
}

int main()
{
	GlRunner *runner = new GlRunner(RenderCB, 2, 2);

	GLuint MStex;

	glActiveTexture(GL_TEXTURE0);
	glGenTextures(1, &MStex);
	glBindTexture(GL_TEXTURE_2D_MULTISAMPLE, MStex);
	glTexStorage2DMultisample(GL_TEXTURE_2D_MULTISAMPLE, 4, GL_RGBA8, 2, 2, GL_TRUE);

	GLuint MSfbo;
	glGenFramebuffers(1, &MSfbo);
	glBindFramebuffer(GL_FRAMEBUFFER, MSfbo);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D_MULTISAMPLE, MStex, 0);
	assert(GL_FRAMEBUFFER_COMPLETE == glCheckFramebufferStatus(GL_FRAMEBUFFER));

	GLuint SSbo;
	glGenBuffers(1, &SSbo);
	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 0, SSbo);
	glBufferData(GL_SHADER_STORAGE_BUFFER, 16*sizeof(GLint), nullptr, GL_DYNAMIC_DRAW);

	GLubyte *mapped = (GLubyte *)glMapBufferRange(GL_SHADER_STORAGE_BUFFER,
			0, 16*sizeof(GLint), GL_MAP_WRITE_BIT);
	memset(mapped, 0, 16*sizeof(GLint));
	glUnmapBuffer(GL_SHADER_STORAGE_BUFFER);

	/* draw multisample texture first */
	glClearColor(0.0, 0.0, 0.0, 0.0);
	glClearDepthf(1.0);
	glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);

	GLfloat pos_buf[] = {
		-1.0, 1.0,
		1.0, 1.0,
		-1.0, -1.0,
		1.0, -1.0
	};

	GLuint VAO, VBO;
	glGenVertexArrays(1, &VAO);
	glBindVertexArray(VAO);

	glGenBuffers(1, &VBO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(pos_buf), pos_buf, GL_STATIC_DRAW);
	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2*sizeof(GLfloat), (GLvoid*)0);
	glEnableVertexAttribArray(0);

	GLuint VS = runner->BuildShaderProgram("shaders/simple.vert", GL_VERTEX_SHADER);
	GLuint FS = runner->BuildShaderProgram("shaders/simple.frag", GL_FRAGMENT_SHADER);
	GLuint DrawPPO = runner->BuildProgramPipeline();
	glUseProgramStages(DrawPPO, GL_VERTEX_SHADER_BIT, VS);
	glUseProgramStages(DrawPPO, GL_FRAGMENT_SHADER_BIT, FS);
	glProgramUniform4f(FS, glGetUniformLocation(FS, "uColor"), 1.0, 1.0, 1.0, 0.0);

	glDrawArrays(GL_TRIANGLES, 0, 3);

	glFinish();

	/* compute */
	GLuint CS = runner->BuildShaderProgram("shaders/multisample_draw_then_compute.comp", GL_COMPUTE_SHADER);
	GLuint CSppo = runner->BuildProgramPipeline();
	glUseProgramStages(CSppo, GL_COMPUTE_SHADER_BIT, CS);

	glDispatchCompute(1, 1, 1);

	glMemoryBarrier(GL_BUFFER_UPDATE_BARRIER_BIT);

	/* verify */
	for (int k = 0; k < 4; k++) {

		glProgramUniform1i(CS, glGetUniformLocation(CS, "uSampleIndex"), k);
		glDispatchCompute(1, 1, 1);

		glMemoryBarrier(GL_BUFFER_UPDATE_BARRIER_BIT);

		GLint *res = (GLint *)glMapBufferRange(GL_SHADER_STORAGE_BUFFER,
				0, 16*sizeof(GLint), GL_MAP_WRITE_BIT);

		std::cout << "######## Sample Plane (" << k << ") #######" << std::endl;
		for (int i = 1; i >= 0; i--) {
			for (int j = 0; j < 8; j++) {
				if (j%4 == 0) {
					std::cout << "(";
				}
				std::cout << (int)res[i*8+j];
				if (j%4 == 3) {
					std::cout << ")";
				} else {
					std::cout << ", ";
				}
			}
			std::cout << std::endl;
		}

		glUnmapBuffer(GL_SHADER_STORAGE_BUFFER);
	}


	return 0;
}
