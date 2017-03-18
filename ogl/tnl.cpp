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
GLuint mvp_uni;
vector<glm::vec3> pos_buf;
vector<glm::vec3> nor_buf;
char obj_name[] = "objs/bunny.obj";

glm::mat4 scale_mat;
glm::mat4 translate_mat;
glm::vec3 camera_location(-2, 1, 3);
glm::mat4 view_mat;
glm::mat4 proj_mat;
glm::mat4 MVP_mat;

void load_obj(const char *path, vector<glm::vec3> &vertices, vector<glm::vec3> &normal)
{
	vector<unsigned int> vertexIndices, normalIndices;
	vector<glm::vec3> temp_vertices;
	vector<glm::vec3> temp_normals;
	char line[128];
	FILE *file = fopen(path, "r");

	while (EOF != fscanf(file, "%s", line)) {

		if (strcmp(line, "v") == 0) {
			glm::vec3 vertex;
			fscanf(file, "%f %f %f\n", &vertex.x, &vertex.y, &vertex.z);
			temp_vertices.push_back(vertex);
		} else if (strcmp(line, "vt") == 0) {
			// placehold handle uv
		} else if (strcmp(line, "vn") == 0) {
			glm::vec3 normal;
			fscanf(file, "%f %f %f\n", &normal.x, &normal.y, &normal.z);
			temp_normals.push_back(normal);
		} else if (strcmp(line, "f") == 0 ) {
			unsigned int vertexIndex[3], normalIndex[3], uvIndex[3];
			int matches = fscanf(file, "%d//%d %d//%d %d//%d\n", &vertexIndex[0], &normalIndex[0], &vertexIndex[1], &normalIndex[1], &vertexIndex[2], &normalIndex[2] );

			vertexIndices.push_back(vertexIndex[0]);
			vertexIndices.push_back(vertexIndex[1]);
			vertexIndices.push_back(vertexIndex[2]);
			normalIndices.push_back(normalIndex[0]);
			normalIndices.push_back(normalIndex[1]);
			normalIndices.push_back(normalIndex[2]);
		}
	}

	for (unsigned int i=0; i<vertexIndices.size(); i++) {
		vertices.push_back(temp_vertices[vertexIndices[i] -1]);
	}
}

static void error_cb(int error, const char* description)
{
	fputs(description, stderr);
	fputs("\n", stderr);
}

void key_cb(GLFWwindow* win, int key, int scancode, int action, int mode)
{
	static float delta = 0.0f;
	static glm::vec3 axis(1.0f, 0.0f, 0.0f);

	if ((key == GLFW_KEY_ESCAPE) && action == GLFW_PRESS)
		glfwSetWindowShouldClose(win, GL_TRUE);

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
		"uniform mat4 mvp;"
		"void main()"
		"{"
		"gl_Position = mvp * vec4(position.x, position.y, position.z, 1.0);"
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
	glGenVertexArrays(1, &g_vao);
	glBindVertexArray(g_vao);

	glGenBuffers(1, &g_vbo);
	glBindBuffer(GL_ARRAY_BUFFER, g_vbo);
	glBufferData(GL_ARRAY_BUFFER, pos_buf.size() * 3 * sizeof(float), &pos_buf[0], GL_STATIC_DRAW);

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), (GLvoid*)0);
	glEnableVertexAttribArray(0);
}

int main(int argc, char **argv)
{
	load_obj(obj_name, pos_buf, nor_buf);

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

	while (!glfwWindowShouldClose(win))
	{
		glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
		glClearDepth(1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		glfwPollEvents();

		glDrawArrays(GL_TRIANGLES, 0, pos_buf.size());

		glfwSwapBuffers(win);
	}

	glfwTerminate();
	return 0;
}
