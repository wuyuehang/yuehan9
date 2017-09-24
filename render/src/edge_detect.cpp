#include <SOIL/SOIL.h>
#include "glrunner.h"
#include <ctime>

GLuint FS;

clock_t start_time;

void RenderCB(GlRunner *runner)
{
	glClearColor(0.2, 0.1, 0.2, 0.0);
	glClearDepthf(1.0);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	clock_t current_time = clock();
	float delta = 0.00001 *(current_time - start_time);

	glProgramUniform1f(FS, glGetUniformLocation(FS, "uTime"), delta);

	glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
}

int main()
{
	start_time = clock();

	GlRunner *runner = new GlRunner(RenderCB);

	GLuint VS = runner->BuildShaderProgram("shaders/quad.vert", GL_VERTEX_SHADER);
	FS = runner->BuildShaderProgram("shaders/edge_detect.frag", GL_FRAGMENT_SHADER);
	GLuint PPO = runner->BuildProgramPipeline();

	glUseProgramStages(PPO, GL_VERTEX_SHADER_BIT, VS);
	glUseProgramStages(PPO, GL_FRAGMENT_SHADER_BIT, FS);

	glProgramUniform1i(FS, glGetUniformLocation(FS, "tColor2D"), 0);

	GLfloat pos_buf[] = {
		-1.0, 1.0,
		1.0, 1.0,
		-1.0, -1.0,
		1.0, -1.0
	};

	GLfloat texc_buf[] = {
		0.0, 0.0,
		1.0, 0.0,
		0.0, 1.0,
		1.0, 1.0
	};

	GLuint VBO[2];
	glGenBuffers(2, VBO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO[0]);
	glBufferData(GL_ARRAY_BUFFER, sizeof(pos_buf), pos_buf, GL_STATIC_DRAW);
	glBindBuffer(GL_ARRAY_BUFFER, VBO[1]);
	glBufferData(GL_ARRAY_BUFFER, sizeof(texc_buf), texc_buf, GL_STATIC_DRAW);

	GLuint VAO;
	glGenVertexArrays(1, &VAO);
	glBindVertexArray(VAO);

	glEnableVertexAttribArray(0);
	glEnableVertexAttribArray(1);

	glVertexAttribFormat(0, 2, GL_FLOAT, GL_FALSE, 0);
	glVertexAttribFormat(1, 2, GL_FLOAT, GL_FALSE, 0);

	glVertexAttribBinding(0, 0);
	glVertexAttribBinding(1, 1);

	glBindVertexBuffer(0, VBO[0], 0, 2*sizeof(GLfloat));
	glBindVertexBuffer(1, VBO[1], 0, 2*sizeof(GLfloat));

	int w, h, c;
	unsigned char *datum = SOIL_load_image("materials/stormtrooper.jpg", &w, &h, &c, SOIL_LOAD_RGBA);

	GLuint srcTexObj;
	glGenTextures(1, &srcTexObj);

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, srcTexObj);

	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, w, h, 0, GL_RGBA, GL_UNSIGNED_BYTE, datum);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

	SOIL_free_image_data(datum);

	runner->OnRender();

	return 0;
}
