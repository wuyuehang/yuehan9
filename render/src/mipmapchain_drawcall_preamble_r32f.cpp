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
					nullptr);
		}

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

		w = std::max(w >> 1, 1);
		h = std::max(h >> 1, 1);
	}

	// prepare vertex data
	GLfloat pos_buf[] = {
		-1., -1.,
		-1., 1.,
		1., 1.,
		-1., -1.,
		1., 1.,
		1., -1.
	};

	GLfloat texc_buf[] = {
		0., 0.,
		0., 1.,
		1., 1.,
		0., 0.,
		1., 1.,
		1., 0.
	};

	GLuint VBO[2];
	GLuint VAO;
	glGenVertexArrays(1, &VAO);
	glBindVertexArray(VAO);

	glGenBuffers(2, VBO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO[0]);
	glBufferData(GL_ARRAY_BUFFER, sizeof(pos_buf), pos_buf, GL_STATIC_DRAW);
	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2*sizeof(GLfloat), (GLvoid*)0);
	glEnableVertexAttribArray(0);

	glBindBuffer(GL_ARRAY_BUFFER, VBO[1]);
	glBufferData(GL_ARRAY_BUFFER, sizeof(texc_buf), texc_buf, GL_STATIC_DRAW);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 2*sizeof(GLfloat), (GLvoid*)0);
	glEnableVertexAttribArray(1);

	// prepare program
	GLuint VS = runner->BuildShaderProgram("shaders/quad.vert", GL_VERTEX_SHADER);
	GLuint FS = runner->BuildShaderProgram("shaders/quad.frag", GL_FRAGMENT_SHADER);
	GLuint PPO = runner->BuildProgramPipeline();
	glUseProgramStages(PPO, GL_VERTEX_SHADER_BIT, VS);
	glUseProgramStages(PPO, GL_FRAGMENT_SHADER_BIT, FS);

	GLuint FBO;
	glGenFramebuffers(1, &FBO);
	glBindFramebuffer(GL_FRAMEBUFFER, FBO);

	glActiveTexture(GL_TEXTURE0);

	w = _mipmap_w;
	h = _mipmap_h;

	for (int k = 0; k < _num_mipmap_chain; k++) {

		w = std::max(w >> 1, 1);
		h = std::max(h >> 1, 1);

		glBindTexture(GL_TEXTURE_2D, srcTexObj[k]);
		glProgramUniform1i(FS, glGetUniformLocation(FS, "color2D"), 0);

		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, srcTexObj[k+1], 0);
		assert(GL_FRAMEBUFFER_COMPLETE == glCheckFramebufferStatus(GL_FRAMEBUFFER));

		glBindFramebuffer(GL_DRAW_FRAMEBUFFER, FBO);

		// reset the right viewport
		glViewport(0, 0, w, h);
		glClearColor(0.0, 0.0, 0.0, 0.0);
		glClear(GL_COLOR_BUFFER_BIT);

		glDrawArrays(GL_TRIANGLES, 0, 6);

		glFinish();

		// verify from framebuffer data on FBO
		GLfloat res[w*h];
		glReadPixels(0, 0, w, h, GL_RED, GL_FLOAT, res);

		std::cout << "######## framebuffer pixel data level(" << k+1 << ")#######" << std::endl;

		for (int i = 0; i < w; i++) {
			for (int j = 0; j < h; j++) {
				std::cout << res[i*w+j] << ", ";
			}
			std::cout << std::endl;
		}

		// verify from texture image data
		glBindTexture(GL_TEXTURE_2D, srcTexObj[k+1]);
		GLfloat res2[w*h];

		glGetTexImage(GL_TEXTURE_2D,
				0, // base level
				GL_RED, // format
				GL_FLOAT, // type
				res2);

		std::cout << "######## target texture image data level(" << k+1 << ") ########" << std::endl;

		for (int i = 0; i < w; i++) {
			for (int j = 0; j < h; j++) {
				std::cout << res2[i*w+j] << ", ";
			}
			std::cout << std::endl;
		}
	}

	return 0;
}
