#include <stdio.h>
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <assert.h>

#include "utility.h"

extern GLboolean glewExperimental;

GLuint g_program;
GLuint g_vao;
GLuint g_vbo;
GLuint g_uni;
GLuint g_qry;
GLuint g_time_qry;

static void error_cb(int error, const char* description)
{
	fputs(description, stderr);
	fputs("\n", stderr);
}

void key_cb(GLFWwindow* win, int key, int scancode, int action, int mode)
{
	if ((key & GLFW_KEY_ESCAPE) && action == GLFW_PRESS)
		glfwSetWindowShouldClose(win, GL_TRUE);
}

void compile_shader(void)
{
	GLuint vertex_shader;
	GLuint fragment_shader;
	GLint status;
	GLchar *source;
	GLchar message[512];

	vertex_shader = glCreateShader(GL_VERTEX_SHADER);
	load_shader_from_file("shaders/simple.vert", &source);
	glShaderSource(vertex_shader, 1, &source, NULL);
	glCompileShader(vertex_shader);

	glGetShaderiv(vertex_shader, GL_COMPILE_STATUS, &status);
	if (!status) {
		glGetShaderInfoLog(vertex_shader, 512, NULL, message);
		printf("%s\n", message);
	}

	fragment_shader = glCreateShader(GL_FRAGMENT_SHADER);
	free(source);
	load_shader_from_file("shaders/simple.frag", &source);
	glShaderSource(fragment_shader, 1, &source, NULL);
	glCompileShader(fragment_shader);

	glGetShaderiv(fragment_shader, GL_COMPILE_STATUS, &status);
	if (!status) {
		glGetShaderInfoLog(fragment_shader, 512, NULL, message);
		printf("%s\n", message);
	}

	g_program = glCreateProgram();
	glAttachShader(g_program, vertex_shader);
	glAttachShader(g_program, fragment_shader);
	glLinkProgram(g_program);

	glGetProgramiv(g_program, GL_LINK_STATUS, &status);
	if (!status) {
		glGetProgramInfoLog(g_program, 512, NULL, message);
		printf("%s\n", message);
	}

	glUseProgram(g_program);
	glDeleteShader(vertex_shader);
	glDeleteShader(fragment_shader);
}

void create_resource(void)
{
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

	glGenVertexArrays(1, &g_vao);
	glBindVertexArray(g_vao);

	glGenBuffers(1, &g_vbo);
	glBindBuffer(GL_ARRAY_BUFFER, g_vbo);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vb_pos), vb_pos, GL_STATIC_DRAW);

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), (GLvoid*)0);
	glEnableVertexAttribArray(0);

	glBindBuffer(GL_ARRAY_BUFFER, 0);

	glBindVertexArray(0);
}

int main(int argc, char **argv)
{
	int width, height;

	glfwSetErrorCallback(error_cb);

	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	glfwWindowHint(GLFW_RESIZABLE, GL_FALSE);

	GLFWwindow* win = glfwCreateWindow(16, 16, __FILE__, NULL, NULL);

	glfwMakeContextCurrent(win);

	glfwSetKeyCallback(win, key_cb);

	glewExperimental = GL_TRUE;
	glewInit();

	glfwGetFramebufferSize(win, &width, &height);
	glViewport(0, 0, width, height);

	compile_shader();

	create_resource();

	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LESS);

	glGenQueries(1, &g_time_qry);
	glGenQueries(1, &g_qry);

	{
		glBeginQuery(GL_TIME_ELAPSED, g_time_qry);
		glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
		glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);

		glBindVertexArray(g_vao);
		g_uni = glGetUniformLocation(g_program, "uColor");
		assert(g_uni != 0xFFFFFFFF);

		GLint result;
		glUniform4f(g_uni, 1.0, 0.0, 0.0, 1.0);
		glBeginQuery(GL_SAMPLES_PASSED, g_qry);
		glDrawArrays(GL_TRIANGLES, 0, 3);
		glEndQuery(GL_SAMPLES_PASSED);
		glGetQueryObjectiv(g_qry, GL_QUERY_RESULT, &result);
		printf("1st uncovered triangle, samples result %d\n", result);

		glUniform4f(g_uni, 0.0, 0.0, 1.0, 1.0);
		glBeginQuery(GL_SAMPLES_PASSED, g_qry);
		glDrawArrays(GL_TRIANGLES, 3, 3);
		glEndQuery(GL_SAMPLES_PASSED);
		glGetQueryObjectiv(g_qry, GL_QUERY_RESULT, &result);
		printf("2nd occlusion triangle, samples result %d\n", result);

		glUniform4f(g_uni, 0.0, 0.0, 1.0, 1.0);
		glBeginQuery(GL_SAMPLES_PASSED, g_qry);
		glDrawArrays(GL_TRIANGLES, 6, 3);
		glEndQuery(GL_SAMPLES_PASSED);
		glGetQueryObjectiv(g_qry, GL_QUERY_RESULT, &result);
		printf("3rd uncovered triangle, samples result %d\n", result);

		glUniform4f(g_uni, 1.0, 0.0, 1.0, 1.0);
		glBeginQuery(GL_SAMPLES_PASSED, g_qry);
		glDrawArrays(GL_TRIANGLES, 9, 3);
		glEndQuery(GL_SAMPLES_PASSED);
		glGetQueryObjectiv(g_qry, GL_QUERY_RESULT, &result);
		printf("4th partial uncovered triangle, samples result %d\n", result);

		// test GL_ANY_SAMPLES_PASSED
		GLuint qry;
		glGenQueries(1, &qry);
		glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
		glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);

		glUniform4f(g_uni, 1.0, 0.0, 0.0, 1.0);
		glBeginQuery(GL_ANY_SAMPLES_PASSED, qry);
		glDrawArrays(GL_TRIANGLES, 0, 3);
		glEndQuery(GL_ANY_SAMPLES_PASSED);
		glGetQueryObjectiv(qry, GL_QUERY_RESULT, &result);
		printf("anycover_test, 1st uncovered triangle, samples result %d\n", result);

		glUniform4f(g_uni, 0.0, 0.0, 1.0, 1.0);
		glBeginQuery(GL_ANY_SAMPLES_PASSED, qry);
		glDrawArrays(GL_TRIANGLES, 3, 3);
		glEndQuery(GL_ANY_SAMPLES_PASSED);
		glGetQueryObjectiv(qry, GL_QUERY_RESULT, &result);
		printf("anycover_test, 2nd occlusion triangle, samples result %d\n", result);
		glEndQuery(GL_TIME_ELAPSED);

		glGetQueryObjectiv(g_time_qry, GL_QUERY_RESULT, &result);
		printf("time elapsed, result %d\n", result);
	}

	glDeleteVertexArrays(1, &g_vao);
	glDeleteBuffers(1, &g_vbo);
	glfwTerminate();

	return 0;
}
