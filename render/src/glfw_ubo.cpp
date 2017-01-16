#include <stdio.h>
#include <string.h>
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

extern GLboolean glewExperimental;

const GLchar* vs_src = "#version 330 core\n"
		"layout (location = 0) in vec3 vposition;"
		"layout (location = 1) in vec4 vcolor;"
		"uniform MVP {"
		"mat4 uModel;"
		"mat4 uView;"
		"mat4 uProj;"
		"};"
		"out vec4 fcolor;"
		"void main()"
		"{"
		"mat4 vMVP = uProj * uView * uModel;"
		"gl_Position = vMVP * vec4(vposition.x, vposition.y, vposition.z, 1.0);"
		"fcolor = vcolor;"
		"}";

const GLchar* fs_src = "#version 330 core\n"
		"out vec4 color;"
		"in vec4 fcolor;"
		"void main()"
		"{"
		"color = fcolor;"
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

	GLuint vao;
	glGenVertexArrays(1, &vao);
	glBindVertexArray(vao);

	/* setup array buffer for position and color
	 */
	GLuint vbo_pos;
	glGenBuffers(1, &vbo_pos);
	glBindBuffer(GL_ARRAY_BUFFER, vbo_pos);
	glBufferData(GL_ARRAY_BUFFER, sizeof(pos_buf), pos_buf, GL_STATIC_DRAW);

	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), (GLvoid *)0);

	GLuint vbo_color;
	glGenBuffers(1, &vbo_color);
	glBindBuffer(GL_ARRAY_BUFFER, vbo_color);
	glBufferData(GL_ARRAY_BUFFER, sizeof(color_buf), color_buf, GL_STATIC_DRAW);

	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, 4 * sizeof(GLfloat), (GLvoid *)0);

	/* setup index buffer for index
	 */
	GLuint vbo_index;
	glGenBuffers(1, &vbo_index);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, vbo_index);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(index_buf), index_buf, GL_STATIC_DRAW);

	/* setup shader */
	GLuint vertex_shader = glCreateShader(GL_VERTEX_SHADER);
	glShaderSource(vertex_shader, 1, &vs_src, NULL);
	glCompileShader(vertex_shader);

	GLuint fragment_shader = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(fragment_shader, 1, &fs_src, NULL);
	glCompileShader(fragment_shader);

	GLuint program = glCreateProgram();
	glAttachShader(program, vertex_shader);
	glAttachShader(program, fragment_shader);
	glLinkProgram(program);
	glUseProgram(program);

	/* updating uniform block */
	glm::mat4 model_mat = glm::mat4(1.0f);

	glm::mat4 view_mat = glm::lookAt(
			glm::vec3(-2, 1, 2),
			glm::vec3(0, 0, 0),
			glm::vec3(0, 1, 0)
	);

	glm::mat4 proj_mat = glm::perspective(glm::radians(45.0f), 1.0f, 0.1f, 100.0f);

	GLuint ubo_blk_idx = glGetUniformBlockIndex(program, "MVP");
	GLint ubo_sz;
	glGetActiveUniformBlockiv(program, ubo_blk_idx, GL_UNIFORM_BLOCK_DATA_SIZE, &ubo_sz);

	const char *names[3] = {"uModel", "uView", "uProj"};
	GLuint indices[3];
	GLint size[3];
	GLint offset[3];
	GLint type[3];

	glGetUniformIndices(program, 3, names, indices);
	glGetActiveUniformsiv(program, 3, indices, GL_UNIFORM_OFFSET, offset);
	glGetActiveUniformsiv(program, 3, indices, GL_UNIFORM_SIZE, size);
	glGetActiveUniformsiv(program, 3, indices, GL_UNIFORM_TYPE, type);

	/* mat4 here is 4x4=16 GLfloat */
	GLubyte *buffer = (GLubyte *)malloc(ubo_sz);
	memcpy(buffer + offset[0], &model_mat[0][0], size[0] * sizeof(GLfloat) * 16);
	memcpy(buffer + offset[1], &view_mat[0][0], size[1] * sizeof(GLfloat) * 16);
	memcpy(buffer + offset[2], &proj_mat[0][0], size[2] * sizeof(GLfloat) * 16);

	GLuint ubo;
	glGenBuffers(1, &ubo);
	glBindBuffer(GL_UNIFORM_BUFFER, ubo);
	glBufferData(GL_UNIFORM_BUFFER, ubo_sz, buffer, GL_STATIC_DRAW);
	glBindBufferBase(GL_UNIFORM_BUFFER, ubo_blk_idx, ubo);

	while (!glfwWindowShouldClose(win))
	{
		glfwPollEvents();

		glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
		glClear(GL_COLOR_BUFFER_BIT);

		glDrawElements(GL_TRIANGLE_STRIP, 4, GL_UNSIGNED_SHORT, (void*)0);

		glfwSwapBuffers(win);
	}

	glDeleteBuffers(1, &vbo_pos);
	glDeleteBuffers(1, &vbo_color);
	glDeleteBuffers(1, &vbo_index);
	glDeleteBuffers(1, &ubo);
	glDeleteVertexArrays(1, &vao);
	glDeleteShader(vertex_shader);
	glDeleteShader(fragment_shader);
	glDeleteProgram(program);

	glfwTerminate();

	return 0;
}
