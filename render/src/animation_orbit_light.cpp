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
		"layout (location = 1) in vec3 normal;"
		"uniform mat4 world;"
		"uniform mat4 view;"
		"uniform mat4 proj;"
		"out vec3 vs_normal;"
		"void main()"
		"{"
		"gl_Position = proj * view * world * vec4(position.x, position.y, position.z, 1.0);"
		"vs_normal = mat3(world) * normal;"
		"}";

const GLchar* fs_src = "#version 330 core\n"
		"out vec4 color;"
		"in vec3 vs_normal;"
		"uniform vec4 orbitlightdir;"
		"void main()"
		"{"
		"color = clamp(dot(orbitlightdir.xyz, vs_normal) * vec4(0.8f, 0.4f, 0.8f, 1.0f), vec4(0.0f, 0.0f, 0.0f, 1.0f), vec4(1.0f, 1.0f, 1.0f, 1.0f));"
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

	GLfloat normal_buf[] = {
			// top
			0.0f, 1.0f, 0.0f,
			0.0f, 1.0f, 0.0f,
			0.0f, 1.0f, 0.0f,
			0.0f, 1.0f, 0.0f,
			0.0f, 1.0f, 0.0f,
			0.0f, 1.0f, 0.0f,
			// bottom
			0.0f, -1.0f, 0.0f,
			0.0f, -1.0f, 0.0f,
			0.0f, -1.0f, 0.0f,
			0.0f, -1.0f, 0.0f,
			0.0f, -1.0f, 0.0f,
			0.0f, -1.0f, 0.0f,
			// front
			0.0f, 0.0f, 1.0f,
			0.0f, 0.0f, 1.0f,
			0.0f, 0.0f, 1.0f,
			0.0f, 0.0f, 1.0f,
			0.0f, 0.0f, 1.0f,
			0.0f, 0.0f, 1.0f,
			// back
			0.0f, 0.0f, -1.0f,
			0.0f, 0.0f, -1.0f,
			0.0f, 0.0f, -1.0f,
			0.0f, 0.0f, -1.0f,
			0.0f, 0.0f, -1.0f,
			0.0f, 0.0f, -1.0f,
			// left
			-1.0f, 0.0f, 0.0f,
			-1.0f, 0.0f, 0.0f,
			-1.0f, 0.0f, 0.0f,
			-1.0f, 0.0f, 0.0f,
			-1.0f, 0.0f, 0.0f,
			-1.0f, 0.0f, 0.0f,
			// right
			1.0f, 0.0f, 0.0f,
			1.0f, 0.0f, 0.0f,
			1.0f, 0.0f, 0.0f,
			1.0f, 0.0f, 0.0f,
			1.0f, 0.0f, 0.0f,
			1.0f, 0.0f, 0.0f,
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

	GLuint vbo_normal;
	glGenBuffers(1, &vbo_normal);
	glBindBuffer(GL_ARRAY_BUFFER, vbo_normal);
	glBufferData(GL_ARRAY_BUFFER, sizeof(normal_buf), normal_buf, GL_STATIC_DRAW);

	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), (GLvoid *)0);

	/* setup shader */
	GLchar message[512];
	GLint status;

	GLuint vertex_shader = glCreateShader(GL_VERTEX_SHADER);
	glShaderSource(vertex_shader, 1, &vs_src, NULL);
	glCompileShader(vertex_shader);

	glGetShaderiv(vertex_shader, GL_COMPILE_STATUS, &status);
	if (!status) {
		glGetShaderInfoLog(vertex_shader, 512, NULL, message);
		printf("%s\n", message);
	}

	GLuint fragment_shader = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(fragment_shader, 1, &fs_src, NULL);
	glCompileShader(fragment_shader);

	glGetShaderiv(fragment_shader, GL_COMPILE_STATUS, &status);
	if (!status) {
		glGetShaderInfoLog(fragment_shader, 512, NULL, message);
		printf("%s\n", message);
	}

	GLuint program = glCreateProgram();
	glAttachShader(program, vertex_shader);
	glAttachShader(program, fragment_shader);
	glLinkProgram(program);

	glGetProgramiv(program, GL_LINK_STATUS, &status);
	if (!status) {
		glGetProgramInfoLog(program, 512, NULL, message);
		printf("%s\n", message);
	}

	glUseProgram(program);

	GLuint world_uni = glGetUniformLocation(program, "world");
	GLuint view_uni = glGetUniformLocation(program, "view");
	GLuint proj_uni = glGetUniformLocation(program, "proj");
	GLuint orbit_light_dir_uni = glGetUniformLocation(program, "orbitlightdir");

	glDeleteShader(vertex_shader);
	glDeleteShader(fragment_shader);

	glm::mat4 view_mat = glm::lookAt(
			glm::vec3(-2, 2, 4),
			glm::vec3(0, 0, 0),
			glm::vec3(0, 1, 0)
	);

	glm::mat4 proj_mat = glm::perspective(glm::radians(60.0f), 1.0f, 0.01f, 100.0f);

	glUniformMatrix4fv(view_uni, 1, GL_FALSE, &view_mat[0][0]);
	glUniformMatrix4fv(proj_uni, 1, GL_FALSE, &proj_mat[0][0]);

	while (!glfwWindowShouldClose(win))
	{
		glfwPollEvents();

		glClearColor(0.12f, 0.34f, 0.56f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		/* recalculate model world matrix */
		struct timeval tv;
		gettimeofday(&tv, NULL);
		static long int startup = 0;

		if (0 == startup)
			startup = tv.tv_sec * 1000000 + tv.tv_usec;

		float gap = (tv.tv_sec * 1000000 + tv.tv_usec - startup) / 80000.0f;

		/* prepare matrix for orbit light */
		glm::mat4 rotate_mat = glm::rotate(glm::mat4(1.0f), glm::radians(gap), glm::vec3(0.0f, 1.0f, 0.0f));

		glm::mat4 translate_mat = glm::translate(glm::vec3(2.0f, 0.0f, 0.0f));

		glm::mat4 world_mat = rotate_mat * translate_mat;

		glm::vec4 orbit_lightdir(-0.5f, 0.5f, 0.0f, 1.0f);

		/* parallel to xoz */
		orbit_lightdir = rotate_mat * glm::vec4(-0.5f, 0.5f, 0.0f, 1.0f);

		glUniform4fv(orbit_light_dir_uni, 1, &orbit_lightdir[0]);

		/* prepare matrix for drawing center box */
		glm::mat4 scale_mat = glm::scale(glm::vec3(1.0f, 1.0f, 1.0f));

		rotate_mat = glm::rotate(glm::mat4(1.0f), glm::radians(gap), glm::vec3(1.0f, 0.0f, 0.0f));

		world_mat = scale_mat;

		glUniformMatrix4fv(world_uni, 1, GL_FALSE, &world_mat[0][0]);

		glDrawArrays(GL_TRIANGLES, 0, 3 * 36);

		glfwSwapBuffers(win);
	}

	glDeleteBuffers(1, &vbo_pos);
	glDeleteBuffers(1, &vbo_normal);
	glDeleteVertexArrays(1, &vao);

	glfwTerminate();

	return 0;
}
