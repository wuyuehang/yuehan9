#include "glrunner.h"
#include <cstdlib>
#include <vector>
#include <ctime>

using std::vector;

#define _num_sierpinski	100000

void RenderCB(GlRunner *runner)
{
	glClearColor(0.2, 0.1, 0.2, 0.0);
	glClearDepthf(1.0);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glDrawArrays(GL_POINTS, 0, _num_sierpinski);
}

int main()
{
	std::srand(std::time(0));

	GlRunner *runner = new GlRunner(RenderCB);

	GLuint VS = runner->BuildShaderProgram("shaders/simple.vert", GL_VERTEX_SHADER);
	GLuint FS = runner->BuildShaderProgram("shaders/simple.frag", GL_FRAGMENT_SHADER);
	GLuint PPO = runner->BuildProgramPipeline();
	glUseProgramStages(PPO, GL_VERTEX_SHADER_BIT, VS);
	glUseProgramStages(PPO, GL_FRAGMENT_SHADER_BIT, FS);

	glProgramUniform4f(FS, glGetUniformLocation(FS, "uColor"), 0.75, 0.0, 0.0, 0.0);

	/* cpu domain generate sierpinski gasket */
	GLfloat leading_vertice[] = {
		-1.0, -1.0,
		0.0, 1.0,
		1.0, -1.0
	};

	GLfloat next_point[] = { 0.0, 0.0 };

	vector<GLfloat> sierpinski_set;

	for (int i = 0; i < _num_sierpinski; i++) {
		int next_index = std::rand() % 3;
		next_index *= 2;

		next_point[0] = 0.5*(next_point[0] + leading_vertice[next_index]);
		next_point[1] = 0.5*(next_point[1] + leading_vertice[next_index+1]);

		sierpinski_set.push_back(next_point[0]);
		sierpinski_set.push_back(next_point[1]);
	}

	/* loading input attribute */
	GLuint VAO, VBO;
	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &VBO);

	glBindVertexArray(VAO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat)*sierpinski_set.size(), &sierpinski_set[0], GL_STATIC_DRAW);

	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2*sizeof(GLfloat), (GLvoid *)0);

	runner->OnRender();

	return 0;
}
