#include "glrunner.h"
#include <ctime>

GLuint FS;

clock_t start_time;

void RenderCB(GlRunner *runner)
{
	glClearColor(0.0, 0.0, 0.0, 0.0);
	glClearDepthf(1.0);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	clock_t current_time = clock();
	float delta = 0.00001 *(current_time - start_time);

	//std::cout << delta << std::endl;

	glProgramUniform1f(FS, glGetUniformLocation(FS, "uTime"), delta);

	double xpos, ypos;
	glfwGetCursorPos(runner->win(), &xpos, &ypos);
	xpos = std::max(0.0, std::min(1.0, xpos / GR_WIDTH));
	ypos = std::max(0.0, std::min(1.0, ypos / GR_HEIGHT));

	glProgramUniform2f(FS, glGetUniformLocation(FS, "uMouse"), 2.0*float(xpos)-1.0, 2*float(ypos)-1.0);

	glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
}

int main(int argc, char **argv)
{
	start_time = clock();

	GlRunner *runner = new GlRunner(RenderCB);

	GLuint VAO, VBO;
	glGenVertexArrays(1, &VAO);
	glBindVertexArray(VAO);

	glGenBuffers(1, &VBO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);

	GLfloat dualtri[] = {
		-1.0, 1.0,
		-1.0, -1.0,
		1.0, 1.0,
		1.0, -1.0
	};

	glBufferData(GL_ARRAY_BUFFER, sizeof(dualtri), dualtri, GL_STATIC_DRAW);

	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2*sizeof(GLfloat), (GLvoid *)0);
	glEnableVertexAttribArray(0);

	GLuint VS = runner->BuildShaderProgram("shaders/simple.vert", GL_VERTEX_SHADER);

	if (argc == 1) {
		FS = runner->BuildShaderProgram("shaders/pixel_karate.frag", GL_FRAGMENT_SHADER);
	} else {
		FS = runner->BuildShaderProgram(argv[1], GL_FRAGMENT_SHADER);
	}

	GLuint PPO = runner->BuildProgramPipeline();
	glUseProgramStages(PPO, GL_VERTEX_SHADER_BIT, VS);
	glUseProgramStages(PPO, GL_FRAGMENT_SHADER_BIT, FS);

	glProgramUniform2ui(FS, glGetUniformLocation(FS, "uResInfo"), GR_WIDTH, GR_HEIGHT);

	runner->OnRender();

	return 0;
}
