#include "ogl_helper.h"

GLuint g_program;
GLuint g_vao[2];
GLuint g_vbo[2];

void compile_shader(void)
{
	GLuint vertex_shader;
	GLuint fragment_shader;
	GLint status;
	GLchar message[512];
	GLchar *glsl = NULL;

	vertex_shader = glCreateShader(GL_VERTEX_SHADER);
	load_shader_from_file("shaders/simple.vert", &glsl);
	glShaderSource(vertex_shader, 1, &glsl, NULL);
	glCompileShader(vertex_shader);

	glGetShaderiv(vertex_shader, GL_COMPILE_STATUS, &status);
	if (!status) {
		glGetShaderInfoLog(vertex_shader, 512, NULL, message);
		printf("%s\n", message);
	}

	fragment_shader = glCreateShader(GL_FRAGMENT_SHADER);
	free(glsl);
	load_shader_from_file("shaders/subroutine.frag", &glsl);
	glShaderSource(fragment_shader, 1, &glsl, NULL);
	glCompileShader(fragment_shader);

	glGetShaderiv(fragment_shader, GL_COMPILE_STATUS, &status);
	if (!status) {
		glGetShaderInfoLog(fragment_shader, 512, NULL, message);
		printf("%s\n", message);
	}
	free(glsl);

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
}

void create_resource(void)
{
	GLfloat tri_down[] = {
			-0.5f, -0.5f, 0.0f,
			 0.5f, -0.5f, 0.0f,
			 0.0f,  0.0f, 0.0f
	};

	GLfloat tri_up[] = {
			 0.5f,  0.5f, 0.0f,
			-0.5f,  0.5f, 0.0f,
			 0.0f,  0.0f, 0.0f
	};

	glGenVertexArrays(2, g_vao);
	glGenBuffers(2, g_vbo);

	glBindVertexArray(g_vao[0]);
	glBindBuffer(GL_ARRAY_BUFFER, g_vbo[0]);
	glBufferData(GL_ARRAY_BUFFER, sizeof(tri_down), tri_down, GL_STATIC_DRAW);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), (GLvoid*)0);
	glEnableVertexAttribArray(0);

	glBindVertexArray(g_vao[1]);
	glBindBuffer(GL_ARRAY_BUFFER, g_vbo[1]);
	glBufferData(GL_ARRAY_BUFFER, sizeof(tri_up), tri_up, GL_STATIC_DRAW);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), (GLvoid*)0);
	glEnableVertexAttribArray(0);
}

int main(int argc, char **argv)
{
	glfwSetErrorCallback(error_cb);

	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	glfwWindowHint(GLFW_RESIZABLE, GL_FALSE);

	GLFWwindow *win = glfwCreateWindow(500, 500, __FILE__, NULL, NULL);

	glfwMakeContextCurrent(win);

	glfwSetKeyCallback(win, key_cb);

	glewExperimental = GL_TRUE;
	glewInit();

	glViewport(0, 0, 500, 500);
	glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
	glClear(GL_COLOR_BUFFER_BIT);

	compile_shader();

	GLuint red_idx = glGetSubroutineIndex(g_program, GL_FRAGMENT_SHADER, "gen_red");
	GLuint blue_idx = glGetSubroutineIndex(g_program, GL_FRAGMENT_SHADER, "gen_blue");

	create_resource();

	while (!glfwWindowShouldClose(win))
	{
		glfwPollEvents();

		glUniformSubroutinesuiv(GL_FRAGMENT_SHADER, 1, &blue_idx);
		glBindVertexArray(g_vao[0]);
		glDrawArrays(GL_TRIANGLES, 0, 3);

		glUniformSubroutinesuiv(GL_FRAGMENT_SHADER, 1, &red_idx);
		glBindVertexArray(g_vao[1]);
		glDrawArrays(GL_TRIANGLES, 0, 3);

		glfwSwapBuffers(win);
	}

	glfwTerminate();

	return 0;
}
