#include <vector>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtx/transform.hpp>
#include <glm/gtx/rotate_vector.hpp>
#include <glm/gtc/matrix_transform.hpp>

#define WIDTH 1280
#define HEIGHT 1024

using namespace std;

GLuint g_program;
GLuint g_vao;
GLuint g_vbo;
GLuint g_ibo;
GLuint mvp_uni;
GLuint inner_uni;
GLuint outter_uni;

glm::mat4 scale_mat;
glm::mat4 translate_mat;
glm::vec3 camera_location(-2, 1, 3);
glm::mat4 view_mat;
glm::mat4 proj_mat;
glm::mat4 MVP_mat;

static void error_cb(int error, const char* description)
{
	fputs(description, stderr);
	fputs("\n", stderr);
}

void key_cb(GLFWwindow* win, int key, int scancode, int action, int mode)
{
	static float delta = 0.0f;
	static glm::vec3 axis(1.0f, 0.0f, 0.0f);
	static int curr_inner = 1;
	static int curr_outter = 1;

	if ((key == GLFW_KEY_ESCAPE) && action == GLFW_PRESS)
		glfwSetWindowShouldClose(win, GL_TRUE);

	// inc inner tess level
	if ((key == GLFW_KEY_A) && action == GLFW_PRESS) {
		curr_inner++;
		glUniform1f(inner_uni, float(curr_inner));
		return;
	}

	// dec inner tess level
	if ((key == GLFW_KEY_S) && action == GLFW_PRESS) {
		curr_inner--;
		if (curr_inner == 0)
			curr_inner= 1;

		glUniform1f(inner_uni, float(curr_inner));
		return;
	}

	// inc outter tess level
	if ((key == GLFW_KEY_D) && action == GLFW_PRESS) {
		curr_outter++;
		glUniform1f(outter_uni, float(curr_outter));
		return;
	}

	// dec outter tess level
	if ((key == GLFW_KEY_F) && action == GLFW_PRESS) {
		curr_outter--;
		if (curr_outter == 0)
			curr_outter= 1;

		glUniform1f(outter_uni, float(curr_outter));
		return;
	}

	if ((key == GLFW_KEY_LEFT) && action == GLFW_PRESS) {
		delta = 1.0f;
		axis = glm::vec3(0.0f, 1.0f, 0.0f);
	}

	if ((key == GLFW_KEY_RIGHT) && action == GLFW_PRESS) {
		delta = -1.0f;
		axis = glm::vec3(0.0f, 1.0f, 0.0f);
	}

	if ((key == GLFW_KEY_UP) && action == GLFW_PRESS) {
		delta = 1.0f;
		axis = glm::vec3(1.0f, 0.0f, 0.0f);
	}

	if ((key == GLFW_KEY_DOWN) && action == GLFW_PRESS) {
		delta = -1.0f;
		axis = glm::vec3(1.0f, 0.0f, 0.0f);
	}

	glm::mat4 rotate_mat = glm::rotate(glm::mat4(1.0f), glm::radians(delta), axis);
	camera_location = glm::mat3(rotate_mat) * camera_location,
	view_mat = glm::lookAt(
			camera_location,
			glm::vec3(0, 0, 0),
			glm::vec3(0, 1, 0)
	);
	MVP_mat = proj_mat * view_mat;

	glUniformMatrix4fv(mvp_uni, 1, GL_FALSE, &MVP_mat[0][0]);
}

const GLchar* vs_src = "#version 430 core\n"
		"layout (location = 0) in vec3 position;"
		"out vec3 tcs_pos_in;"
		"void main()"
		"{"
		"tcs_pos_in = position;"
		"}";

const GLchar* tcs_src = "#version 430 core\n"
		"layout (vertices = 3) out;"
		"in vec3 tcs_pos_in[];"
		"out vec3 tes_pos_in[];"
		"uniform float inner;"
		"uniform float outter;"
		"void main()"
		"{"
		"tes_pos_in[gl_InvocationID] = tcs_pos_in[gl_InvocationID];"
		"gl_TessLevelOuter[0] = outter;"
		"gl_TessLevelOuter[1] = outter;"
		"gl_TessLevelOuter[2] = outter;"
		"gl_TessLevelInner[0] = inner;"
		"}";

const GLchar* tes_src = "#version 430 core\n"
		"layout (triangles, equal_spacing, ccw) in;"
		"in vec3 tes_pos_in[];"
		"uniform mat4 mvp;"
		"vec3 interpolate(vec3 v0, vec3 v1, vec3 v2)"
		"{"
		"return vec3(gl_TessCoord.x) * v0 + vec3(gl_TessCoord.y) * v1 + vec3(gl_TessCoord.z) * v2;"
		"}"
		"void main()"
		"{"
		"gl_Position = mvp * vec4(interpolate(tes_pos_in[0], tes_pos_in[1], tes_pos_in[2]), 1.0f);"
		"}";

const GLchar* fs_src = "#version 430 core\n"
		"out vec4 color;"
		"void main()"
		"{"
		"color = vec4(1.0f, 1.0f, 0.0f, 1.0f);"
		"}";

void compile_shader(void)
{
	GLuint vertex_shader;
	GLuint fragment_shader;
	GLuint tcs_shader;
	GLuint tes_shader;
	GLint status;
	GLchar message[512];

	vertex_shader = glCreateShader(GL_VERTEX_SHADER);
	glShaderSource(vertex_shader, 1, &vs_src, NULL);
	glCompileShader(vertex_shader);

	glGetShaderiv(vertex_shader, GL_COMPILE_STATUS, &status);
	if (!status) {
		glGetShaderInfoLog(vertex_shader, 512, NULL, message);
		printf("%s\n", message);
	}

	tcs_shader = glCreateShader(GL_TESS_CONTROL_SHADER);
	glShaderSource(tcs_shader, 1, &tcs_src, NULL);
	glCompileShader(tcs_shader);

	glGetShaderiv(tcs_shader, GL_COMPILE_STATUS, &status);
	if (!status) {
		glGetShaderInfoLog(tcs_shader, 512, NULL, message);
		printf("%s\n", message);
	}

	tes_shader = glCreateShader(GL_TESS_EVALUATION_SHADER);
	glShaderSource(tes_shader, 1, &tes_src, NULL);
	glCompileShader(tes_shader);

	glGetShaderiv(tes_shader, GL_COMPILE_STATUS, &status);
	if (!status) {
		glGetShaderInfoLog(tes_shader, 512, NULL, message);
		printf("%s\n", message);
	}

	fragment_shader = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(fragment_shader, 1, &fs_src, NULL);
	glCompileShader(fragment_shader);

	glGetShaderiv(fragment_shader, GL_COMPILE_STATUS, &status);
	if (!status) {
		glGetShaderInfoLog(fragment_shader, 512, NULL, message);
		printf("%s\n", message);
	}

	g_program = glCreateProgram();
	glAttachShader(g_program, vertex_shader);
	glAttachShader(g_program, tcs_shader);
	glAttachShader(g_program, tes_shader);
	glAttachShader(g_program, fragment_shader);
	glLinkProgram(g_program);

	glGetProgramiv(g_program, GL_LINK_STATUS, &status);
	if (!status) {
		glGetProgramInfoLog(g_program, 512, NULL, message);
		printf("%s\n", message);
	}

	glUseProgram(g_program);
}

void create_resource(void)
{
	const GLushort ib[] = {
			2, 1, 0,
			3, 2, 0,
			4, 3, 0,
			5, 4, 0,
			1, 5, 0,
			11, 6, 7,
			11, 7, 8,
			11, 8, 9,
			11, 9, 10,
			11, 10, 6,
			1, 2, 6,
			2, 3, 7,
			3, 4, 8,
			4, 5, 9,
			5, 1, 10,
			2, 7, 6,
			3, 8, 7,
			4, 9, 8,
			5, 10, 9,
			1, 6, 10 };

	const GLfloat vb_pos[] = {
			0.000f,  0.000f,  1.000f,
			0.894f,  0.000f,  0.447f,
			0.276f,  0.851f,  0.447f,
			-0.724f,  0.526f,  0.447f,
			-0.724f, -0.526f,  0.447f,
			0.276f, -0.851f,  0.447f,
			0.724f,  0.526f, -0.447f,
			-0.276f,  0.851f, -0.447f,
			-0.894f,  0.000f, -0.447f,
			-0.276f, -0.851f, -0.447f,
			0.724f, -0.526f, -0.447f,
			0.000f,  0.000f, -1.000f };

	glGenVertexArrays(1, &g_vao);
	glBindVertexArray(g_vao);

	glGenBuffers(1, &g_vbo);
	glBindBuffer(GL_ARRAY_BUFFER, g_vbo);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vb_pos), vb_pos, GL_STATIC_DRAW);

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), (GLvoid*)0);
	glEnableVertexAttribArray(0);

	glGenBuffers(1, &g_ibo);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, g_ibo);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(ib), ib, GL_STATIC_DRAW);
}

int main(int argc, char **argv)
{
	glfwSetErrorCallback(error_cb);

	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	glfwWindowHint(GLFW_RESIZABLE, GL_FALSE);

	GLFWwindow *win = glfwCreateWindow(WIDTH, HEIGHT, __FILE__, NULL, NULL);

	glfwMakeContextCurrent(win);

	glfwSetKeyCallback(win, key_cb);

	glewExperimental = GL_TRUE;
	glewInit();

	glViewport(0, 0, WIDTH, HEIGHT);
	glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LESS);
	glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	glEnable(GL_CULL_FACE);
	glCullFace(GL_BACK);
	glFrontFace(GL_CCW);

	compile_shader();

	mvp_uni = glGetUniformLocation(g_program, "mvp");
	assert(mvp_uni != -1);

	inner_uni = glGetUniformLocation(g_program, "inner");
	assert(inner_uni != -1);

	outter_uni = glGetUniformLocation(g_program, "outter");
	assert(outter_uni != -1);

	create_resource();

	// view transform
	view_mat = glm::lookAt(
			camera_location,
			glm::vec3(0, 0, 0),
			glm::vec3(0, 1, 0)
	);

	// projection transform
	proj_mat = glm::perspective(glm::radians(60.0f), 1.0f, 1.0f, 100.0f);

	// combine Model-View-Projection
	MVP_mat = proj_mat * view_mat;

	glUniformMatrix4fv(mvp_uni, 1, GL_FALSE, &MVP_mat[0][0]);
	glUniform1f(inner_uni, 1.0f);
	glUniform1f(outter_uni, 3.0f);

	while (!glfwWindowShouldClose(win))
	{
		glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
		glClearDepth(1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		glfwPollEvents();

		glDrawElements(GL_PATCHES, 60, GL_UNSIGNED_SHORT, (void*)0);

		glfwSwapBuffers(win);
	}

	glfwTerminate();
	return 0;
}
