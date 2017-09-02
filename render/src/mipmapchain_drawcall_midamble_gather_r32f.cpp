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

	// preapare source texture
	GLfloat srcData[_mipmap_w*_mipmap_h];

	std::cout << "######## original data #######" << std::endl;
	for (int i = 0; i < _mipmap_w; i++) {
		for (int j = 0; j < _mipmap_h; j++) {
			srcData[i*_mipmap_w+j] = i*_mipmap_w+j;
			std::cout << srcData[i*_mipmap_w+j] << ", ";
		}
		std::cout << std::endl;
	}

	GLuint srcTexObj;
	glGenTextures(1, &srcTexObj);
	glBindTexture(GL_TEXTURE_2D, srcTexObj);

	glTexStorage2D(GL_TEXTURE_2D,
			_num_mipmap_chain + 1, // number of level
			GL_R32F, // internal format
			_mipmap_w, // w
			_mipmap_h);

	glTexSubImage2D(GL_TEXTURE_2D,
			0, // level
			0, // xoffset
			0, // yoffset
			_mipmap_w,
			_mipmap_h,
			GL_RED, // format
			GL_FLOAT, // type
			srcData);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR_MIPMAP_LINEAR);

	// prepare vertex data
	GLfloat pos_buf[] = {
		-1., -1.,
		-1., 1.,
		1., 1.,
		-1., -1.,
		1., 1.,
		1., -1.
	};

	GLuint VBO;
	GLuint VAO;
	glGenVertexArrays(1, &VAO);
	glBindVertexArray(VAO);

	glGenBuffers(1, &VBO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(pos_buf), pos_buf, GL_STATIC_DRAW);
	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2*sizeof(GLfloat), (GLvoid*)0);
	glEnableVertexAttribArray(0);

	// prepare program
	GLuint VS = runner->BuildShaderProgram("shaders/simple.vert", GL_VERTEX_SHADER);
	GLuint FS = runner->BuildShaderProgram("shaders/pot_quad.frag", GL_FRAGMENT_SHADER);
	GLuint PPO = runner->BuildProgramPipeline();
	glUseProgramStages(PPO, GL_VERTEX_SHADER_BIT, VS);
	glUseProgramStages(PPO, GL_FRAGMENT_SHADER_BIT, FS);

	GLuint FBO;
	glGenFramebuffers(1, &FBO);
	glBindFramebuffer(GL_FRAMEBUFFER, FBO);

	glActiveTexture(GL_TEXTURE0);

	int w = _mipmap_w;
	int h = _mipmap_h;

	for (int k = 0; k < _num_mipmap_chain; k++) {

		w = std::max(w >> 1, 1);
		h = std::max(h >> 1, 1);

		glProgramUniform1i(FS, glGetUniformLocation(FS, "color2D"), 0);
		glProgramUniform1i(FS, glGetUniformLocation(FS, "srcLOD"), k);

		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, srcTexObj, k+1);
		assert(GL_FRAMEBUFFER_COMPLETE == glCheckFramebufferStatus(GL_FRAMEBUFFER));

		glBindFramebuffer(GL_DRAW_FRAMEBUFFER, FBO);

		// reset the right viewport
		glViewport(0, 0, w, h);
		glClearColor(0.0, 0.0, 0.0, 0.0);
		glClear(GL_COLOR_BUFFER_BIT);

		glDrawArrays(GL_TRIANGLES, 0, 6);

		// verify from texture image data
		GLfloat res[w*h];

		glGetTexImage(GL_TEXTURE_2D,
				k+1, // base level
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
