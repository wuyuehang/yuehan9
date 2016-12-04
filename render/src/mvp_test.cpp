#include <stdio.h>
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

extern GLboolean glewExperimental;

const GLchar* vs_src = "#version 330 core\n"
		"layout (location = 0) in vec3 position;"
		"layout (location = 1) in vec4 color;"
		"uniform mat4 mvp;"
		"out vec4 frag_color;"
		"void main()"
		"{"
		"gl_Position = mvp * vec4(position.x, position.y, position.z, 1.0);"
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
	GLuint vao;
	GLuint vbo_index, vbo_pos, vbo_color;
	GLuint vertex_shader, fragment_shader;
	GLuint program;
	GLuint mvp_uni;

	glfwInit();

	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	GLFWwindow* win = glfwCreateWindow(500, 500, "mvp on dual tri", NULL, NULL);
	glfwMakeContextCurrent(win);
	glfwSetKeyCallback(win, key_cb);

	glewExperimental = GL_TRUE;
	glewInit();

	glViewport(0, 0, 500, 500);

	GLfloat pos_buf[] = {
			-0.25f, 0.25f, 0.0f,
			0.25f, -0.25f, 0.0f,
			-0.25f, -0.25f, 0.0f,
			0.25f, 0.25f, 0.0f
	};

	GLfloat color_buf[] = {
			1.0f, 0.0f, 0.0f, 0.0f,
			0.0f, 1.0f, 0.0f, 0.0f,
			0.0f, 0.0f, 1.0f, 0.0f,
			1.0f, 1.0f, 0.0f, 0.0f,
	};

	GLushort index_buf[] = {
			2, 0, 1, 3
	};

	glGenVertexArrays(1, &vao);
	glBindVertexArray(vao);

	/* setup array buffer for position and color
	 */
	glGenBuffers(1, &vbo_pos);
	glBindBuffer(GL_ARRAY_BUFFER, vbo_pos);
	glBufferData(GL_ARRAY_BUFFER, sizeof(pos_buf), pos_buf, GL_STATIC_DRAW);

	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), (GLvoid *)0);

	glGenBuffers(1, &vbo_color);
	glBindBuffer(GL_ARRAY_BUFFER, vbo_color);
	glBufferData(GL_ARRAY_BUFFER, sizeof(color_buf), color_buf, GL_STATIC_DRAW);

	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, 4 * sizeof(GLfloat), (GLvoid *)0);

	/* setup index buffer for index
	 */
	glGenBuffers(1, &vbo_index);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, vbo_index);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(index_buf), index_buf, GL_STATIC_DRAW);

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

	mvp_uni = glGetUniformLocation(program, "mvp");

	glDeleteShader(vertex_shader);
	glDeleteShader(fragment_shader);

	while (!glfwWindowShouldClose(win))
	{
		glm::mat4 model_mat = glm::mat4(1.0f);

		glm::mat4 view_mat = glm::lookAt(
				glm::vec3(-2, 1, 2),
				glm::vec3(0, 0, 0),
				glm::vec3(0, 1, 0)
		);

		/* 45 degree of fov, width/height ratio, 0.1f near, 100.f far
		 */
		glm::mat4 proj_mat = glm::perspective(glm::radians(45.0f), 1.0f, 0.1f, 100.0f);

		glm::mat4 mvp_mat = proj_mat * view_mat * model_mat;

		glUniformMatrix4fv(mvp_uni, 1, GL_FALSE, &mvp_mat[0][0]);

		glfwPollEvents();

		glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
		glClear(GL_COLOR_BUFFER_BIT);

		glDrawElements(GL_TRIANGLE_STRIP, 4, GL_UNSIGNED_SHORT, (void*)0);

		glfwSwapBuffers(win);
	}

	glDeleteBuffers(1, &vbo_pos);
	glDeleteBuffers(1, &vbo_color);
	glDeleteBuffers(1, &vbo_index);
	glDeleteVertexArrays(1, &vao);

	glfwTerminate();

	return 0;
}
