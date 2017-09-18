#include "glrunner.h"
#include <cstdlib>
#include <vector>
#include <ctime>

GLuint GS;

clock_t start_time;
bool freezen_frame = false;

#define _mutidraw_cout	1000

void RenderCB(GlRunner *runner)
{
	if (freezen_frame == true) {
		;
	} else {
		glClearColor(0.2, 0.1, 0.2, 0.0);
		glClearDepthf(1.0);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		/* since the limitation of emission of geometry shader,
		 * we take use mutiple draw and with one clean to simulate
		 */
		for (int i = 0; i < _mutidraw_cout; i++) {
			clock_t current_time = clock();
			float delta = 0.00001 *(current_time - start_time);

			glProgramUniform1f(GS, glGetUniformLocation(GS, "uTime"), delta);

			glDrawArrays(GL_TRIANGLES, 0, 3);
		}

		freezen_frame = true;
	}
}

int main()
{
	start_time = clock();

	std::srand(std::time(0));

	GlRunner *runner = new GlRunner(RenderCB);

	GLuint VS = runner->BuildShaderProgram("shaders/simple.vert", GL_VERTEX_SHADER);
	GS = runner->BuildShaderProgram("shaders/gpu_gasket2d.geo", GL_GEOMETRY_SHADER);
	GLuint FS = runner->BuildShaderProgram("shaders/simple.frag", GL_FRAGMENT_SHADER);
	GLuint PPO = runner->BuildProgramPipeline();
	glUseProgramStages(PPO, GL_VERTEX_SHADER_BIT, VS);
	glUseProgramStages(PPO, GL_GEOMETRY_SHADER_BIT, GS);
	glUseProgramStages(PPO, GL_FRAGMENT_SHADER_BIT, FS);

	glProgramUniform4f(FS, glGetUniformLocation(FS, "uColor"), 0.75, 0.0, 0.0, 0.0);

	GLfloat leading_vertice[] = {
		-1.0, -1.0,
		0.0, 1.0,
		1.0, -1.0
	};

	/* loading input attribute */
	GLuint VAO, VBO;
	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &VBO);

	glBindVertexArray(VAO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(leading_vertice), leading_vertice, GL_STATIC_DRAW);

	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2*sizeof(GLfloat), (GLvoid *)0);

	runner->OnRender();

	return 0;
}
