#include <stdio.h>
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtx/transform.hpp>

extern GLboolean glewExperimental;

GLuint g_program;
GLuint g_vao;
GLuint g_vbo;
GLuint g_translate;

static void error_cb(int error, const char* description)
{
	fputs(description, stderr);
	fputs("\n", stderr);
}

void key_cb(GLFWwindow* win, int key, int scancode, int action, int mode)
{
	if (key & (GLFW_KEY_Q | GLFW_KEY_ESCAPE) && action == GLFW_PRESS)
		glfwSetWindowShouldClose(win, GL_TRUE);
}

const GLchar* vs_src = "#version 330 core\n"
		"layout (location = 0) in vec3 position;"
		"uniform mat4 translate;"
		"void main()"
		"{"
		"gl_Position = translate * vec4(position.x, position.y, position.z, 1.0);"
		"}";

const GLchar* fs_src = "#version 330 core\n"
		"out vec4 color;"
		"void main()"
		"{"
		"color = vec4(1.0f, 0.5f, 0.2f, 1.0f);"
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

	g_translate = glGetUniformLocation(g_program, "translate");
	assert(g_translate != 0xFFFFFFFF);
}

void create_resource(void)
{
	GLfloat vb_pos[] = {
			-0.5f, -0.5f, 0.0f,
			 0.5f, -0.5f, 0.0f,
			 0.0f,  0.5f, 0.0f
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

void render(void)
{
	/* translate target triangle in +0.5x, +0.5y, +0.5z, that is right corner */
	glm::mat4 translate_mat = glm::translate(glm::vec3(0.5f, 0.5f, 0.5f));

	glUniformMatrix4fv(g_translate, 1, GL_FALSE, &translate_mat[0][0]);
	glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
	glClear(GL_COLOR_BUFFER_BIT);

	glBindVertexArray(g_vao);
	glDrawArrays(GL_TRIANGLES, 0, 3);
	glBindVertexArray(0);
}

void cleanup(void)
{
	glDeleteVertexArrays(1, &g_vao);
	glDeleteBuffers(1, &g_vbo);
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

	GLFWwindow* win = glfwCreateWindow(500, 500, "translate test", NULL, NULL);

	glfwMakeContextCurrent(win);

	glfwSetKeyCallback(win, key_cb);

	glewExperimental = GL_TRUE;
	glewInit();

	glfwGetFramebufferSize(win, &width, &height);
	glViewport(0, 0, width, height);

	/* pre render stage */
	compile_shader();

	create_resource();

	/* event loop */
	while (!glfwWindowShouldClose(win))
	{
		glfwPollEvents();

		render();

		glfwSwapBuffers(win);

		/* quit loop for one off test */
		//glfwSetWindowShouldClose(win, GL_TRUE);
	}

	/* pre exit clean up stage */
	cleanup();

	glfwTerminate();

	return 0;
}
