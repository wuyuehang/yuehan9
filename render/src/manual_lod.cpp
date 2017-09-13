#include "glrunner.h"
#include <glm/gtx/transform.hpp>
#include <glm/gtx/rotate_vector.hpp>
#include <glm/gtc/matrix_transform.hpp>

#define _sz	128
#define _num_of_level	8
#define _scale	0.25

void RenderCB(GlRunner *runner)
{

}

int main()
{
	GlRunner *runner = new GlRunner(RenderCB, _sz, _sz);

	/* allocate a 64x64 mipmap 2d texture */
	GLuint srcTexObj;
	glGenTextures(1, &srcTexObj);
	glBindTexture(GL_TEXTURE_2D, srcTexObj);
	glTexStorage2D(GL_TEXTURE_2D,
			_num_of_level, // number of levels
			GL_R32F, // internal format
			_sz,
			_sz);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

	/* clear each level to specific data */
	float clear_data = 0.0;

	for (int l = 0; l < _num_of_level; l++) {
		clear_data = float(l);
		glClearTexImage(srcTexObj,
				l, // level
				GL_RED, // format
				GL_FLOAT, // type
				&clear_data);
	}

	/* retrieve each level for verification */
	int w = _sz;
	int h = _sz;
	float sum = 0.0;

	std::cout << std::endl;
	for (int k = 0; k < _num_of_level; k++) {

		sum = 0.0;

		GLfloat *pSrcData = new GLfloat[w*h];

		glGetTexImage(GL_TEXTURE_2D,
				k,
				GL_RED,
				GL_FLOAT,
				pSrcData);

		for (int i = h-1; i >= 0; i--) {
			for (int j = 0; j < w; j++) {
				sum += pSrcData[i*w+j];
			}
		}

		std::cout << "Level (" << k << ") Average of Elements: " << sum / w / h << std::endl;

		delete [] pSrcData;

		w = w >> 1;
		h = h >> 1;
	}
	std::cout << std::endl;

	/* prepare attribute data */
	GLfloat pos[] = {
		-1.0, 1.0,
		1.0, 1.0,
		-1.0, -1.0,
		1.0, -1.0
	};

	GLfloat uv[] = {
		0.0, 1.0,
		1.0, 1.0,
		0.0, 0.0,
		1.0, 0.0
	};

	GLuint VAO, VBO[2];
	glGenVertexArrays(1, &VAO);
	glGenBuffers(2, VBO);

	glBindVertexArray(VAO);

	glBindBuffer(GL_ARRAY_BUFFER, VBO[0]);
	glBufferData(GL_ARRAY_BUFFER, sizeof(pos), pos, GL_STATIC_DRAW);
	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2*sizeof(GLfloat), 0);
	glEnableVertexAttribArray(0);

	glBindBuffer(GL_ARRAY_BUFFER, VBO[1]);
	glBufferData(GL_ARRAY_BUFFER, sizeof(uv), uv, GL_STATIC_DRAW);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 2*sizeof(GLfloat), 0);
	glEnableVertexAttribArray(1);

	/* prepare shaders */
	GLuint VS = runner->BuildShaderProgram("shaders/lod.vert", GL_VERTEX_SHADER);
	GLuint FS = runner->BuildShaderProgram("shaders/manual_lod.frag", GL_FRAGMENT_SHADER);
	GLuint PPO = runner->BuildProgramPipeline();

	glUseProgramStages(PPO, GL_VERTEX_SHADER_BIT, VS);
	glUseProgramStages(PPO, GL_FRAGMENT_SHADER_BIT, FS);

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, srcTexObj);
	glProgramUniform1i(FS, glGetUniformLocation(FS, "mipmapTexture2D"), 0);

	/* allocate a 64x64 single level texture for render */
	GLuint dstTexObj;
	glGenTextures(1, &dstTexObj);
	glBindTexture(GL_TEXTURE_2D, dstTexObj);
	glTexStorage2D(GL_TEXTURE_2D,
			1, // number of levels
			GL_R32F, // internal format
			_sz,
			_sz);

	/* attaching to framebuffer object */
	GLuint FBO;
	glGenFramebuffers(1, &FBO);
	glBindFramebuffer(GL_FRAMEBUFFER, FBO);

	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, dstTexObj, 0);
	assert(GL_FRAMEBUFFER_COMPLETE == glCheckFramebufferStatus(GL_FRAMEBUFFER));

	glViewport(0, 0, _sz, _sz);
	glClearColor(0.0, 0.0, 0.0, 0.0);
	glClearDepthf(1.0);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	/* draw it */
	glm::mat4 scale_mat = glm::scale(glm::vec3(_scale, _scale, _scale));
	glProgramUniformMatrix4fv(VS, glGetUniformLocation(VS, "uWorld"), 1, GL_FALSE, &scale_mat[0][0]);

	glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);

	/* retrieve for verification */
	GLfloat *pRes = new GLfloat[_sz*_sz];

	glGetTexImage(GL_TEXTURE_2D,
			0, // base level
			GL_RED, // format
			GL_FLOAT, // type
			pRes);

	/* sample center */
	std::cout << "Manual Computed LOD: " << pRes[_sz*_sz/2+_sz/2] << std::endl;
	delete [] pRes;

	return 0;
}
