#include <assert.h>
#include <iostream>
#include "glrunner.h"

GLuint g_uni;
GLuint g_qry;
GLuint g_time_qry;

GLuint fs, vs;

void RenderCB(GlRunner *runner)
{
	glBeginQuery(GL_TIME_ELAPSED, g_time_qry);
	glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
	glClearDepthf(1.0);
	glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);

	g_uni = glGetUniformLocation(fs, "uColor");
	assert(g_uni != 0xFFFFFFFF);

	GLint result;
	glProgramUniform4f(fs, g_uni, 1.0, 0.0, 0.0, 1.0);
	glBeginQuery(GL_SAMPLES_PASSED, g_qry);
	glDrawArrays(GL_TRIANGLES, 0, 3);
	glEndQuery(GL_SAMPLES_PASSED);
	glGetQueryObjectiv(g_qry, GL_QUERY_RESULT, &result);
	std::cout <<"1st uncovered triangle, samples result "<< result << std::endl;

	glProgramUniform4f(fs, g_uni, 0.0, 0.0, 1.0, 1.0);
	glBeginQuery(GL_SAMPLES_PASSED, g_qry);
	glDrawArrays(GL_TRIANGLES, 3, 3);
	glEndQuery(GL_SAMPLES_PASSED);
	glGetQueryObjectiv(g_qry, GL_QUERY_RESULT, &result);
	std::cout << "2nd occlusion triangle, samples result " << result << std::endl;

	glProgramUniform4f(fs, g_uni, 0.0, 0.0, 1.0, 1.0);
	glBeginQuery(GL_SAMPLES_PASSED, g_qry);
	glDrawArrays(GL_TRIANGLES, 6, 3);
	glEndQuery(GL_SAMPLES_PASSED);
	glGetQueryObjectiv(g_qry, GL_QUERY_RESULT, &result);
	std::cout << "3rd uncovered triangle, samples result " << result << std::endl;

	glProgramUniform4f(fs, g_uni, 1.0, 0.0, 1.0, 1.0);
	glBeginQuery(GL_SAMPLES_PASSED, g_qry);
	glDrawArrays(GL_TRIANGLES, 9, 3);
	glEndQuery(GL_SAMPLES_PASSED);
	glGetQueryObjectiv(g_qry, GL_QUERY_RESULT, &result);
	std::cout << "4th partial uncovered triangle, samples result " << result << std::endl;

	// test GL_ANY_SAMPLES_PASSED
	GLuint qry;
	glGenQueries(1, &qry);
	glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
	glClearDepthf(1.0);
	glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);

	glProgramUniform4f(fs, g_uni, 1.0, 0.0, 0.0, 1.0);
	glBeginQuery(GL_ANY_SAMPLES_PASSED, qry);
	glDrawArrays(GL_TRIANGLES, 0, 3);
	glEndQuery(GL_ANY_SAMPLES_PASSED);
	glGetQueryObjectiv(qry, GL_QUERY_RESULT, &result);
	std::cout << "anycover_test, 1st uncovered triangle, samples result " << result << std::endl;

	glProgramUniform4f(fs, g_uni, 0.0, 0.0, 1.0, 1.0);
	glBeginQuery(GL_ANY_SAMPLES_PASSED, qry);
	glDrawArrays(GL_TRIANGLES, 3, 3);
	glEndQuery(GL_ANY_SAMPLES_PASSED);
	glGetQueryObjectiv(qry, GL_QUERY_RESULT, &result);
	std::cout << "anycover_test, 2nd occlusion triangle, samples result " << result << std::endl;
	glEndQuery(GL_TIME_ELAPSED);

	glGetQueryObjectiv(g_time_qry, GL_QUERY_RESULT, &result);
	std::cout << "time elapsed, result " << result << std::endl;

	glfwSetWindowShouldClose(runner->win(), true);
}

int main(int argc, char **argv)
{
	GlRunner *runner = new GlRunner(RenderCB);

	vs = runner->BuildShaderProgram("shaders/simple.vert", GL_VERTEX_SHADER);
	fs = runner->BuildShaderProgram("shaders/simple.frag", GL_FRAGMENT_SHADER);

	GLuint ppline = runner->BuildProgramPipeline();

	glUseProgramStages(ppline, GL_VERTEX_SHADER_BIT, vs);
	glUseProgramStages(ppline, GL_FRAGMENT_SHADER_BIT, fs);

	// preamble resource
	GLfloat vb_pos[] = {
			// red tri
			-0.5f, -0.5f, 0.3f,
			 0.5f, -0.5f, 0.3f,
			 0.0f,  0.5f, 0.3f,
			 // blue tri
			-0.5f, -0.5f, 0.5f,
			 0.5f, -0.5f, 0.5f,
			 0.0f,  0.5f, 0.5f,
			// green tri
			-0.5f, -0.5f, 0.1f,
			 0.5f, -0.5f, 0.1f,
			 0.0f,  0.5f, 0.1f,
			 // purple tri
			-0.5f, 0.5f, 0.5f,
			0.5f, 0.5f, 0.5f,
			0.0f, -0.5f, 0.5f,
	};

	GLuint VBO, VAO;
	glGenVertexArrays(1, &VAO);
	glBindVertexArray(VAO);

	glGenBuffers(1, &VBO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vb_pos), vb_pos, GL_STATIC_DRAW);

	glEnableVertexAttribArray(0);
	glVertexAttribFormat(0, 3, GL_FLOAT, GL_FALSE, 3*sizeof(GLfloat));
	glVertexAttribBinding(0, 0);
	glBindVertexBuffer(0, VBO, 0, 3*sizeof(GLfloat));

	glGenQueries(1, &g_time_qry);
	glGenQueries(1, &g_qry);

	runner->OnRender();

	return 0;
}
