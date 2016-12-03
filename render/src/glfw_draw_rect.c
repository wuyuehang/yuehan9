#include <stdio.h>
#include <GL/glew.h>
#include <GLFW/glfw3.h>

extern GLboolean glewExperimental;

const GLchar* vs_src = "#version 330 core\n"
		"layout (location = 0) in vec3 position;"
		"layout (location = 1) in vec4 color;"
		"out vec4 frag_color;"
		"void main()"
		"{"
		"gl_Position = vec4(position.x, position.y, position.z, 1.0);"
		"frag_color = color;"
		"}";

const GLchar* fs_src = "#version 330 core\n"
		"out vec4 color;"
		"in vec4 frag_color;"
		"void main()"
		"{"
		"color = frag_color;"
		"}";

void key_cb(GLFWwindow* win, int key, int scancode, int action, int mode)
{
	if (key & (GLFW_KEY_Q | GLFW_KEY_ESCAPE) && action == GLFW_PRESS)
		glfwSetWindowShouldClose(win, GL_TRUE);
}

int main(int argc, char **argv)
{
	GLuint vbo, vao;
	GLuint vertex_shader, fragment_shader;
	GLuint program;

	glfwInit();

	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	GLFWwindow* win = glfwCreateWindow(500, 500, "dual tri", NULL, NULL);
	glfwMakeContextCurrent(win);
	glfwSetKeyCallback(win, key_cb);

	glewExperimental = GL_TRUE;
	glewInit();

	glViewport(0, 0, 500, 500);

	/* packed in vec3 (pos) + vec4 (color) */
	GLfloat buffer[] = {
			/* down triangle */
			-0.25f, 0.25f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f,
			0.25f, -0.25f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f,
			-0.25f, -0.25f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f,
			/* up triangle */
			-0.25f, 0.25f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f,
			0.25f, 0.25f, 0.25f, 1.0f, 1.0f, 0.0f, 0.0f,
			0.25f, -0.25f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f
	};

	glGenVertexArrays(1, &vao);
	glBindVertexArray(vao);

	glGenBuffers(1, &vbo);
	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	glBufferData(GL_ARRAY_BUFFER, sizeof(buffer), buffer, GL_STATIC_DRAW);

	/* here we pack position and color buffer into a single
	 * vbo, therefore the offset should be considered when setting
	 * its relative attribute index on glsl side
	 */
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 7 * sizeof(GLfloat), (GLvoid *)0);

	glEnableVertexAttribArray(1);
	/* color buffer content is 3*sizeof(GLfloat) = 12
	 * offset from the vbo
	 */
	glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, 7 * sizeof(GLfloat), (GLvoid *)12);

	/* setup shader */
	vertex_shader = glCreateShader(GL_VERTEX_SHADER);
	glShaderSource(vertex_shader, 1, &vs_src, NULL);
	glCompileShader(vertex_shader);

	fragment_shader = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(fragment_shader, 1, &fs_src, NULL);
	glCompileShader(fragment_shader);

	program = glCreateProgram();
	glAttachShader(program, vertex_shader);
	glAttachShader(program, fragment_shader);
	glLinkProgram(program);
	glUseProgram(program);

	glDeleteShader(vertex_shader);
	glDeleteShader(fragment_shader);

	while (!glfwWindowShouldClose(win))
	{
		glfwPollEvents();

		glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
		glClear(GL_COLOR_BUFFER_BIT);

		glDrawArrays(GL_TRIANGLES, 0, 6);
		glfwSwapBuffers(win);
	}

	glDeleteBuffers(1, &vbo);
	glDeleteVertexArrays(1, &vao);

	glfwTerminate();

	return 0;
}
