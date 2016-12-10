#include <stdio.h>
#include <sys/time.h>
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtx/transform.hpp>
#include <glm/gtx/rotate_vector.hpp>
#include <glm/gtc/matrix_transform.hpp>

extern GLboolean glewExperimental;

const GLchar* vs_src = "#version 330 core\n"
		"layout (location = 0) in vec3 position;"
		"uniform mat4 mvp;"
		"void main()"
		"{"
		"gl_Position = mvp * vec4(position.x, position.y, position.z, 1.0);"
		"}";

const GLchar* fs_src0 = "#version 330 core\n"
		"out vec4 color;"
		"void main()"
		"{"
		"color = vec4(1.0f, 0.0f, 0.0f, 1.0f);"
		"}";

const GLchar* fs_src1 = "#version 330 core\n"
		"out vec4 color;"
		"void main()"
		"{"
		"color = vec4(0.0f, 1.0f, 0.0f, 1.0f);"
		"}";

void key_cb(GLFWwindow* win, int key, int scancode, int action, int mode)
{
	if (key & (GLFW_KEY_Q | GLFW_KEY_ESCAPE) && action == GLFW_PRESS)
		glfwSetWindowShouldClose(win, GL_TRUE);
}

int main(int argc, char **argv)
{
	glfwInit();

	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	GLFWwindow* win = glfwCreateWindow(500, 500, __FILE__, NULL, NULL);
	glfwMakeContextCurrent(win);
	glfwSetKeyCallback(win, key_cb);

	glewExperimental = GL_TRUE;
	glewInit();

	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LESS);
	glViewport(0, 0, 500, 500);

	GLfloat pos_buf[] = {
			// top face
			-0.5f, 0.5f, -0.5f,
			0.5f, 0.5f, -0.5f,
			-0.5f, 0.5f, 0.5f,

			-0.5f, 0.5f, 0.5f,
			0.5f, 0.5f, -0.5f,
			0.5f, 0.5f, 0.5f,

			// bottom face
			-0.5f, -0.5f, -0.5f,
			-0.5f, -0.5f, 0.5f,
			0.5f, -0.5f, -0.5f,

			0.5f, -0.5f, -0.5f,
			-0.5f, -0.5f, 0.5f,
			0.5f, -0.5f, 0.5f,

			// front face
			-0.5f, 0.5f, 0.5f,
			0.5f, 0.5f, 0.5f,
			-0.5f, -0.5f, 0.5f,

			-0.5f, -0.5f, 0.5f,
			0.5f, 0.5f, 0.5f,
			0.5f, -0.5f, 0.5f,

			// back face
			-0.5f, 0.5f, -0.5f,
			-0.5f, -0.5f, -0.5f,
			0.5f, 0.5f, -0.5f,

			0.5f, 0.5f, -0.5f,
			-0.5f, -0.5f, -0.5f,
			0.5f, -0.5f, -0.5f,

			// left face
			-0.5f, 0.5f, 0.5f,
			-0.5f, -0.5f, 0.5f,
			-0.5f, -0.5f, -0.5f,

			-0.5f, 0.5f, 0.5f,
			-0.5f, -0.5f, -0.5f,
			-0.5f, 0.5f, -0.5f,

			// right face
			0.5f, 0.5f, 0.5f,
			0.5f, -0.5f, -0.5f,
			0.5f, -0.5f, 0.5f,

			0.5f, 0.5f, 0.5f,
			0.5f, 0.5f, -0.5f,
			0.5f, -0.5f, -0.5f,
	};

	GLuint vao[2];
	glGenVertexArrays(2, vao);

	/* handle 1st vao for object box */
	glBindVertexArray(vao[0]);

	GLuint vbo_pos;
	glGenBuffers(1, &vbo_pos);
	glBindBuffer(GL_ARRAY_BUFFER, vbo_pos);
	glBufferData(GL_ARRAY_BUFFER, sizeof(pos_buf), pos_buf, GL_STATIC_DRAW);

	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), (GLvoid *)0);

	/* handle 2nd vao for light box */
	glBindVertexArray(vao[1]);

	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), (GLvoid *)0);

	/* setup shader */
	GLuint vertex_shader = glCreateShader(GL_VERTEX_SHADER);
	glShaderSource(vertex_shader, 1, &vs_src, NULL);
	glCompileShader(vertex_shader);

	GLuint fragment_shader[2];
	fragment_shader[0] = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(fragment_shader[0], 1, &fs_src0, NULL);
	glCompileShader(fragment_shader[0]);

	fragment_shader[1] = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(fragment_shader[1], 1, &fs_src1, NULL);
	glCompileShader(fragment_shader[1]);

	GLuint program[2];
	program[0] = glCreateProgram();
	glAttachShader(program[0], vertex_shader);
	glAttachShader(program[0], fragment_shader[0]);
	glLinkProgram(program[0]);

	program[1] = glCreateProgram();
	glAttachShader(program[1], vertex_shader);
	glAttachShader(program[1], fragment_shader[1]);
	glLinkProgram(program[1]);

	GLuint mvp_uni[2];
	mvp_uni[0] = glGetUniformLocation(program[0], "mvp");
	mvp_uni[1] = glGetUniformLocation(program[1], "mvp");

	glm::mat4 view_mat = glm::lookAt(
			glm::vec3(-2, 2, 4),
			glm::vec3(0, 0, 0),
			glm::vec3(0, 1, 0)
	);

	glm::mat4 proj_mat = glm::perspective(glm::radians(60.0f), 1.0f, 0.01f, 100.0f);

	while (!glfwWindowShouldClose(win))
	{
		glfwPollEvents();

		glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		/* recalculate model world matrix */
		struct timeval tv;
		gettimeofday(&tv, NULL);
		static long int startup = 0;

		if (0 == startup)
			startup = tv.tv_sec * 1000000 + tv.tv_usec;

		float gap = (tv.tv_sec * 1000000 + tv.tv_usec - startup) / 10000.0f;

		/* prepare matrix for drawing first orbit box */
		glm::mat4 scale_mat = glm::scale(glm::vec3(0.5f, 0.5f, 0.5f));

		glm::mat4 rotate_mat = glm::rotate(glm::mat4(1.0f), glm::radians(gap), glm::vec3(0.0f, 1.0f, 0.0f));

		glm::mat4 translate_mat = glm::translate(glm::vec3(2.0f, 0.0f, 0.0f));

		glm::mat4 world_mat = rotate_mat * translate_mat * scale_mat;

		glm::mat4 mvp_mat = proj_mat * view_mat * world_mat;

		glBindVertexArray(vao[0]);
		glUseProgram(program[0]);
		glUniformMatrix4fv(mvp_uni[0], 1, GL_FALSE, &mvp_mat[0][0]);

		glDrawArrays(GL_TRIANGLES, 0, 3 * 36);
		glBindVertexArray(0);

		/* prepare matrix for drawing second rotate box */
		scale_mat = glm::scale(glm::vec3(1.0f, 1.0f, 1.0f));

		rotate_mat = glm::rotate(glm::mat4(1.0f), glm::radians(gap / 2.0f), glm::vec3(1.0f, 0.0f, 0.0f));

		world_mat = rotate_mat * scale_mat;

		mvp_mat = proj_mat * view_mat * world_mat;

		glBindVertexArray(vao[1]);
		glUseProgram(program[01]);
		glUniformMatrix4fv(mvp_uni[1], 1, GL_FALSE, &mvp_mat[0][0]);

		glDrawArrays(GL_TRIANGLES, 0, 3 * 36);
		glBindVertexArray(0);

		glfwSwapBuffers(win);
	}

	glDeleteBuffers(1, &vbo_pos);
	glDeleteVertexArrays(2, vao);
	glDeleteShader(vertex_shader);
	glDeleteShader(fragment_shader[0]);
	glDeleteShader(fragment_shader[1]);
	glDeleteProgram(program[0]);
	glDeleteProgram(program[1]);

	glfwTerminate();

	return 0;
}
