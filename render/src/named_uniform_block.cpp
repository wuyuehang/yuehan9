#include "glrunner.h"
#include <cstring>

void RenderCB(GlRunner *runner)
{
	glClearColor(0.1, 0.15, 0.2, 0.0);
	glClearDepthf(1.0);
	glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);

	glDrawArrays(GL_TRIANGLES, 0, 6);
}

int main()
{
	GlRunner *runner = new GlRunner(RenderCB);

	GLuint VS = runner->BuildShaderProgram("shaders/named_uniform_block.vert", GL_VERTEX_SHADER);
	GLuint FS = runner->BuildShaderProgram("shaders/named_uniform_block.frag", GL_FRAGMENT_SHADER);
	GLuint PPO = runner->BuildProgramPipeline();
	glUseProgramStages(PPO, GL_VERTEX_SHADER_BIT, VS);
	glUseProgramStages(PPO, GL_FRAGMENT_SHADER_BIT, FS);

	GLfloat pos_buf[] = {
		-1.0, -1.0,
		-1.0, 1.0,
		1.0, 1.0,
		-1.0, -1.0,
		1.0, 1.0,
		1.0, -1.0
	};

	enum { attr_pos, attr_total };
	GLuint VBO[attr_total];
	glGenBuffers(attr_total, VBO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO[attr_pos]);
	glBufferData(GL_ARRAY_BUFFER, sizeof(pos_buf), pos_buf, GL_STATIC_DRAW);

	GLuint VAO;
	glGenVertexArrays(1, &VAO);
	glBindVertexArray(VAO);

	glEnableVertexAttribArray(attr_pos);
	glVertexAttribFormat(attr_pos, 2, GL_FLOAT, GL_FALSE, 0);
	glVertexAttribBinding(attr_pos, 0);
	glBindVertexBuffer(0, VBO[attr_pos], 0, 2*sizeof(GLfloat));

	// uniform block name, not the instance name
	GLuint UboBlkIdx = glGetUniformBlockIndex(FS, "_WRAP");
	std::cout << "uniform block index: " << UboBlkIdx << std::endl;
	assert(UboBlkIdx != -1);

	GLint UboSize;
	glGetActiveUniformBlockiv(FS, UboBlkIdx, GL_UNIFORM_BLOCK_DATA_SIZE, &UboSize);
	std::cout << "uniform block data size: " << UboSize << std::endl;

	// uniform block name, not the instance name
	const char *names[3] = { "_WRAP.bbox", "_WRAP.color", "_WRAP.resinfo" };
	GLuint indices[3] = {};
	GLint sizes[3] = {};
	GLint offsets[3] = {};
	GLint types[3] = {};

	glGetUniformIndices(FS, 3, names, indices);
	glGetActiveUniformsiv(FS, 3, indices, GL_UNIFORM_OFFSET, offsets);
	glGetActiveUniformsiv(FS, 3, indices, GL_UNIFORM_SIZE, sizes);
	glGetActiveUniformsiv(FS, 3, indices, GL_UNIFORM_TYPE, types);

	for (int i = 0; i < 3; i++) {
		std::cout << names[i] << std::endl;
		std::cout << "indice: " << indices[i] << std::endl;
		std::cout << "offset: " << offsets[i] << std::endl;
		std::cout << "size: " << sizes[i] << std::endl;
		std::cout << "type: " << types[i] << std::endl;
	}

	// lock and load
	GLubyte *datum = (GLubyte *)malloc(UboSize);
	GLuint bbox[4] = {100, 900, 100, 900};
	GLfloat color[4] = {1.0, 0.0, 0.0, 1.0};
	GLuint resinfo[2] = { GR_WIDTH, GR_HEIGHT };

	std::memcpy(datum + offsets[0], &bbox[0], sizes[0]*sizeof(GLuint)*4);
	std::memcpy(datum + offsets[1], &color[0], sizes[1]*sizeof(GLfloat)*4);
	std::memcpy(datum + offsets[2], &resinfo[0], sizes[2]*sizeof(GLuint)*2);

	GLuint UBO;
	glGenBuffers(1, &UBO);
	glBindBuffer(GL_UNIFORM_BUFFER, UBO);
	glBufferData(GL_UNIFORM_BUFFER, UboSize, datum, GL_DYNAMIC_DRAW);
	glBindBufferBase(GL_UNIFORM_BUFFER, UboBlkIdx, UBO);

	runner->OnRender();

	return 0;
}
