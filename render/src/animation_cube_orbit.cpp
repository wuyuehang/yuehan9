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

	GLFWwindow* win = glfwCreateWindow(500, 500, __FILE__, NULL, NULL);
	glfwMakeContextCurrent(win);
	glfwSetKeyCallback(win, key_cb);

	glewExperimental = GL_TRUE;
	glewInit();

	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LESS);
	glViewport(0, 0, 500, 500);

	GLfloat pos_buf[] = {
			-0.5f, 0.5f, -0.5f,
			0.5f, 0.5f, -0.5f,
			-0.5f, 0.5f, 0.5f,
			0.5f, 0.5f, 0.5f,

			-0.5f, -0.5f, -0.5f,
			0.5f, -0.5f, -0.5f,
			-0.5f, -0.5f, 0.5f,
			0.5f, -0.5f, 0.5f,
	};

	GLfloat color_buf[] = {
			1.0f, 0.0f, 0.0f, 0.0f,
			0.0f, 1.0f, 0.0f, 0.0f,
			0.0f, 0.0f, 1.0f, 0.0f,
			1.0f, 0.0f, 0.0f, 0.0f,
			0.0f, 1.0f, 0.0f, 0.0f,
			0.0f, 0.0f, 1.0f, 0.0f,
			1.0f, 0.0f, 1.0f, 0.0f,
			1.0f, 0.0f, 1.0f, 0.0f,
	};

	GLushort index_buf[] = {
			2, 3, 0, 3, 0, 1,	// top face
			6, 7, 4, 7, 4, 5,	// bottom face
			2, 7, 6, 2, 7, 3,	// front face
			0, 4, 5, 0, 5, 1,	// back face
			2, 4, 6, 2, 4, 0,	// left face
			3, 7, 5, 3, 5, 1,	// right face
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

		glm::mat4 view_mat = glm::lookAt(
				glm::vec3(-2, 2, 4),
				glm::vec3(0, 0, 0),
				glm::vec3(0, 1, 0)
		);

		glm::mat4 proj_mat = glm::perspective(glm::radians(60.0f), 1.0f, 0.01f, 100.0f);

		glm::mat4 mvp_mat = proj_mat * view_mat * world_mat;

		glUniformMatrix4fv(mvp_uni, 1, GL_FALSE, &mvp_mat[0][0]);

		glfwPollEvents();

		glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_SHORT, (void*)0);

		/* prepare matrix for drawing second rotate box */
		scale_mat = glm::scale(glm::vec3(1.0f, 1.0f, 1.0f));

		rotate_mat = glm::rotate(glm::mat4(1.0f), glm::radians(gap / 2.0f), glm::vec3(1.0f, 0.0f, 0.0f));

		world_mat = rotate_mat * scale_mat;

		mvp_mat = proj_mat * view_mat * world_mat;

		glUniformMatrix4fv(mvp_uni, 1, GL_FALSE, &mvp_mat[0][0]);

		glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_SHORT, (void*)0);

		glfwSwapBuffers(win);
	}

	glDeleteBuffers(1, &vbo_pos);
	glDeleteBuffers(1, &vbo_color);
	glDeleteBuffers(1, &vbo_index);
	glDeleteVertexArrays(1, &vao);

	glfwTerminate();

	return 0;
}
