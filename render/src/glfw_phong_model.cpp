#include "utility.h"
#include <assert.h>
#include <sys/time.h>
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtx/transform.hpp>
#include <glm/gtx/rotate_vector.hpp>
#include <glm/gtc/matrix_transform.hpp>

extern GLboolean glewExperimental;

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

	/* setup vao & vbo */
	GLuint vao;
	glGenVertexArrays(1, &vao);
	glBindVertexArray(vao);

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

	/* setup shader & program */
	GLchar message[512];
	GLint status;
	GLchar *source = NULL;

	GLuint vertex_shader = glCreateShader(GL_VERTEX_SHADER);
	load_shader_from_file("shaders/glfw_phong.vert", &source);
	glShaderSource(vertex_shader, 1, &source, NULL);
	glCompileShader(vertex_shader);

	glGetShaderiv(vertex_shader, GL_COMPILE_STATUS, &status);
	if (!status) {
		glGetShaderInfoLog(vertex_shader, 512, NULL, message);
		printf("%s\n", message);
	}

	GLuint fragment_shader[2];
	fragment_shader[0] = glCreateShader(GL_FRAGMENT_SHADER);
	free(source);
	load_shader_from_file("shaders/glfw_phong_cube.frag", &source);
	glShaderSource(fragment_shader[0], 1, &source, NULL);
	glCompileShader(fragment_shader[0]);

	glGetShaderiv(fragment_shader[0], GL_COMPILE_STATUS, &status);
	if (!status) {
		glGetShaderInfoLog(fragment_shader[0], 512, NULL, message);
		printf("%s\n", message);
	}

	fragment_shader[1] = glCreateShader(GL_FRAGMENT_SHADER);
	free(source);
	load_shader_from_file("shaders/glfw_phong_light.frag", &source);
	glShaderSource(fragment_shader[1], 1, &source, NULL);
	glCompileShader(fragment_shader[1]);
	free(source);

	glGetShaderiv(fragment_shader[1], GL_COMPILE_STATUS, &status);
	if (!status) {
		glGetShaderInfoLog(fragment_shader[1], 512, NULL, message);
		printf("%s\n", message);
	}

	GLuint cube_prog = glCreateProgram();
	glAttachShader(cube_prog, vertex_shader);
	glAttachShader(cube_prog, fragment_shader[0]);
	glLinkProgram(cube_prog);

	glGetProgramiv(cube_prog, GL_LINK_STATUS, &status);
	if (!status) {
		glGetProgramInfoLog(cube_prog, 512, NULL, message);
		printf("%s\n", message);
	}


	GLuint light_prog = glCreateProgram();
	glAttachShader(light_prog, vertex_shader);
	glAttachShader(light_prog, fragment_shader[1]);
	glLinkProgram(light_prog);

	glGetProgramiv(light_prog, GL_LINK_STATUS, &status);
	if (!status) {
		glGetProgramInfoLog(light_prog, 512, NULL, message);
		printf("%s\n", message);
	}

	/* sanity check symbol */
	GLuint model_uni[2];
	model_uni[0] = glGetUniformLocation(cube_prog, "world");
	model_uni[1] = glGetUniformLocation(light_prog, "world");
	assert(model_uni[0] != -1);
	assert(model_uni[1] != -1);

	GLuint view_uni[2];
	view_uni[0] = glGetUniformLocation(cube_prog, "view");
	view_uni[1] = glGetUniformLocation(light_prog, "view");
	assert(view_uni[0] != -1);
	assert(view_uni[1] != -1);

	GLuint proj_uni[2];
	proj_uni[0] = glGetUniformLocation(cube_prog, "proj");
	proj_uni[1] = glGetUniformLocation(light_prog, "proj");
	assert(proj_uni[0] != -1);
	assert(proj_uni[1] != -1);

	GLuint light_uni;
	light_uni= glGetUniformLocation(cube_prog, "light_pos");
	assert(light_uni != -1);

	GLuint camera_uni;
	camera_uni = glGetUniformLocation(cube_prog, "view_pos");
	assert(camera_uni != -1);

	/* setup unchanged matrixs and vectors */
	glm::vec3 camera_location(-2, 1, 4);

	glm::mat4 view_mat = glm::lookAt(
			camera_location,
			glm::vec3(0, 0, 0),
			glm::vec3(0, 1, 0)
	);

	glm::mat4 proj_mat = glm::perspective(glm::radians(60.0f), 1.0f, 0.01f, 100.0f);

	glUseProgram(cube_prog);
	glUniformMatrix4fv(view_uni[0], 1, GL_FALSE, &view_mat[0][0]);
	glUniformMatrix4fv(proj_uni[0], 1, GL_FALSE, &proj_mat[0][0]);
	glUniform3fv(camera_uni, 1, &camera_location[0]);

	glUseProgram(light_prog);
	glUniformMatrix4fv(view_uni[1], 1, GL_FALSE, &view_mat[0][0]);
	glUniformMatrix4fv(proj_uni[1], 1, GL_FALSE, &proj_mat[0][0]);

	/* game loop */
	while (!glfwWindowShouldClose(win))
	{
		glfwPollEvents();
		glfwSwapBuffers(win);
		glClearColor(0.05f, 0.05f, 0.05f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		/* calculate time delta */
		struct timeval tv;
		gettimeofday(&tv, NULL);
		static long int startup = 0;

		if (0 == startup)
			startup = tv.tv_sec * 1000000 + tv.tv_usec;
		float gap = (tv.tv_sec * 1000000 + tv.tv_usec - startup) / 10000.0f;

		/* setup orbit light, visual light location */
		glm::vec3 light_location(8.0f, 0.0f, 0.0f);
		glm::mat4 scale_mat = glm::scale(glm::vec3(0.25f, 0.25f, 0.25f));
		glm::mat4 rotate_mat = glm::rotate(glm::mat4(1.0f), glm::radians(gap), glm::vec3(0.0f, 1.0f, 0.0f));
		glm::mat4 translate_mat = translate(glm::vec3(2.0f, 0.0f, 0.0f));
		glm::mat4 model_mat = rotate_mat * translate_mat * scale_mat;
		glUseProgram(light_prog);
		glUniformMatrix4fv(model_uni[0], 1, GL_FALSE, &model_mat[0][0]);
		glDrawArrays(GL_TRIANGLES, 0, 36);

		light_location = glm::mat3(rotate_mat) * light_location;
		/* setup cube box */
		scale_mat = glm::mat4(1.0f);
		rotate_mat = glm::mat4(1.0f);
		model_mat = rotate_mat * scale_mat;

		glUseProgram(cube_prog);

		glUniform3fv(light_uni, 1, &light_location[0]);
		glUniformMatrix4fv(model_uni[0], 1, GL_FALSE, &model_mat[0][0]);
		glDrawArrays(GL_TRIANGLES, 0, 36);
	}

	/* cleanup */
	glDeleteShader(vertex_shader);
	glDeleteShader(fragment_shader[0]);
	glDeleteShader(fragment_shader[1]);
	glDeleteProgram(cube_prog);
	glDeleteProgram(light_prog);
	glDeleteBuffers(1, &vbo_pos);
	glDeleteBuffers(1, &vbo_normal);
	glDeleteVertexArrays(1, &vao);
	glfwTerminate();

	return 0;
}
