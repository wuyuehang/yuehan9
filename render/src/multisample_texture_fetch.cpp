#include "glrunner.h"

void RenderCB(GlRunner *runner)
{
}

int main(void)
{
	GlRunner *runner = new GlRunner(RenderCB, 2, 2);

	// prepare multisample texture
	GLuint MSfbo;
	glGenFramebuffers(1, &MSfbo);
	glBindFramebuffer(GL_FRAMEBUFFER, MSfbo);

	GLuint MStex;
	glGenTextures(1, &MStex);
	glBindTexture(GL_TEXTURE_2D_MULTISAMPLE, MStex);
	glTexStorage2DMultisample(GL_TEXTURE_2D_MULTISAMPLE,
			4, // 4x
			GL_RGBA8, // internal format
			2, // w
			2, // h
			GL_TRUE);

	glTexParameteri(GL_TEXTURE_2D_MULTISAMPLE, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D_MULTISAMPLE, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

	glFramebufferTexture2D(GL_FRAMEBUFFER,
			GL_COLOR_ATTACHMENT0,
			GL_TEXTURE_2D_MULTISAMPLE,
			MStex,
			0);

	assert(GL_FRAMEBUFFER_COMPLETE == glCheckFramebufferStatus(GL_FRAMEBUFFER));

	GLfloat pattern[2];
	std::cout << std::endl;
	for (int i = 0; i < 4; i++) {
		glGetMultisamplefv(GL_SAMPLE_POSITION, i, pattern);
		std::cout << "4x Multisample Sample(" << i << ") Position (" << pattern[0] << ", " << pattern[1] << ")\n";
	}
	std::cout << std::endl;

	// prepare normal texture
	GLuint SSfbo;
	glGenFramebuffers(1, &SSfbo);
	glBindFramebuffer(GL_FRAMEBUFFER, SSfbo);

	GLuint SStex;
	glGenTextures(1, &SStex);
	glBindTexture(GL_TEXTURE_2D, SStex);
	glTexImage2D(GL_TEXTURE_2D,
			0, // base level
			GL_RGBA8, // internal format
			2, // w
			2, // h
			0, // border
			GL_RGBA, // format
			GL_UNSIGNED_BYTE, // type
			nullptr);
	glFramebufferTexture2D(GL_FRAMEBUFFER,
			GL_COLOR_ATTACHMENT0,
			GL_TEXTURE_2D,
			SStex,
			0);

	assert(GL_FRAMEBUFFER_COMPLETE == glCheckFramebufferStatus(GL_FRAMEBUFFER));

	// multisample render first
	glBindFramebuffer(GL_FRAMEBUFFER, MSfbo);

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

	// texelFetch onto normal framebuffer then
	glBindFramebuffer(GL_FRAMEBUFFER, SSfbo);

	GLuint MSvs = runner->BuildShaderProgram("shaders/quad.vert", GL_VERTEX_SHADER);
	GLuint MSfs = runner->BuildShaderProgram("shaders/multisample_texture_fetch.frag", GL_FRAGMENT_SHADER);
	GLuint MSppo = runner->BuildProgramPipeline();

	glUseProgramStages(MSppo, GL_VERTEX_SHADER_BIT, MSvs);
	glUseProgramStages(MSppo, GL_FRAGMENT_SHADER_BIT, MSfs);

	// verify
	glBindFramebuffer(GL_READ_FRAMEBUFFER, SSfbo);

	for (int k = 0; k < 4; k++) {

		glProgramUniform1i(MSfs, glGetUniformLocation(MSfs, "uSampleIndex"), k);

		glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);

		GLubyte res[16];
		glReadPixels(0, 0, 2, 2, GL_RGBA, GL_UNSIGNED_BYTE, res);

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
	}
}
