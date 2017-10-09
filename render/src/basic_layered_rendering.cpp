#include "glrunner.h"

void RenderCB(GlRunner *runner)
{

}

int main()
{
	GlRunner *runner = new GlRunner(RenderCB, 1, 1);

	glDisable(GL_DEPTH_TEST);

	/* program object */
	GLuint VS = runner->BuildShaderProgram("shaders/simple.vert", GL_VERTEX_SHADER);
	GLuint GS = runner->BuildShaderProgram("shaders/basic_layered_rendering.geo", GL_GEOMETRY_SHADER);
	GLuint FS = runner->BuildShaderProgram("shaders/attrib2.frag", GL_FRAGMENT_SHADER);

	GLuint PPO = runner->BuildProgramPipeline();

	glUseProgramStages(PPO, GL_VERTEX_SHADER_BIT, VS);
	glUseProgramStages(PPO, GL_GEOMETRY_SHADER_BIT, GS);
	glUseProgramStages(PPO, GL_FRAGMENT_SHADER_BIT, FS);

	assert(glGetUniformLocation(GS, "uNumLayers") != -1);
	glProgramUniform1i(GS, glGetUniformLocation(GS, "uNumLayers"), 4);

	/* vertice content */
	GLfloat pos_buf[] = {
		0.0, 0.0
	};

	GLuint VAO, VBO;
	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &VBO);
	glBindVertexArray(VAO);

	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(pos_buf), pos_buf, GL_STATIC_DRAW);

	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2*sizeof(GLfloat), 0);

	/* render target */
	GLuint FBO, dstTexObj;
	glGenTextures(1, &dstTexObj);
	glBindTexture(GL_TEXTURE_2D_ARRAY, dstTexObj);
	glTexImage3D(GL_TEXTURE_2D_ARRAY,
			0, // base level
			GL_R32F, // internal fmt
			1, // w
			1, // h
			4, // number layer
			0, // border
			GL_RED, // fmt
			GL_FLOAT, // type
			nullptr);

	glGenFramebuffers(1, &FBO);
	glBindFramebuffer(GL_FRAMEBUFFER, FBO);

	glFramebufferTexture(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, dstTexObj, 0);
	assert(GL_FRAMEBUFFER_COMPLETE == glCheckFramebufferStatus(GL_FRAMEBUFFER));

	glClearColor(0.0, 0.0, 0.0, 0.0);
	glClear(GL_COLOR_BUFFER_BIT);

	glDrawArrays(GL_POINTS, 0, 1);

	GLfloat res[4];

	glGetTexImage(GL_TEXTURE_2D_ARRAY,
			0, // level
			GL_RED, // fmt
			GL_FLOAT, // type
			res);

	for (int i = 0; i < 4; i++) {
		std::cout << res[i] << std::endl;
	}

	return 0;
}
