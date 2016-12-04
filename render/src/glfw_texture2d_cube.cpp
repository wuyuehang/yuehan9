#include <stdio.h>
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <SOIL/SOIL.h>

extern GLboolean glewExperimental;

const GLchar* vs_src = "#version 330 core\n"
		"layout (location = 0) in vec3 position;"
		"layout (location = 1) in vec2 uv_cord;"
		"uniform mat4 mvp;"
		"out vec2 UV;"
		"void main()"
		"{"
		"gl_Position = mvp * vec4(position.x, position.y, position.z, 1.0);"
		"UV = uv_cord;"
		"}";

const GLchar* fs_src = "#version 330 core\n"
		"in vec2 UV;"
		"out vec4 COLOR;"
		"uniform sampler2D v4v_01;"
		"void main()"
		"{"
		"COLOR = vec4(texture(v4v_01, UV).rgb, 1.0);"
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
			-0.5f, 0.5f, -0.5f,
			0.5f, 0.5f, -0.5f,
			-0.5f, 0.5f, 0.5f,
			0.5f, 0.5f, 0.5f,

			-0.5f, -0.5f, -0.5f,
			0.5f, -0.5f, -0.5f,
			-0.5f, -0.5f, 0.5f,
			0.5f, -0.5f, 0.5f,

			-0.5f, 0.5f, 0.5f,
			0.5f, 0.5f, 0.5f,
			-0.5f, -0.5f, 0.5f,
			0.5f, -0.5f, 0.5f,

			-0.5f, 0.5f, -0.5f,
			0.5f, 0.5f, -0.5f,
			-0.5f, -0.5f, -0.5f,
			0.5f, -0.5f, -0.5f,

			-0.5f, 0.5f, -0.5f,
			-0.5f, 0.5f, 0.5f,
			-0.5f, -0.5f, -0.5f,
			-0.5f, -0.5f, 0.5f,

			0.5f, 0.5f, 0.5f,
			0.5f, 0.5f, -0.5f,
			0.5f, -0.5f, 0.5f,
			0.5f, -0.5f, -0.5f,
	};

	GLfloat texcord_buf[] = {
			0.0f, 0.0f,	// top face
			1.0f, 0.0f,
			0.0f, 1.0f,
			1.0f, 1.0f,
			0.0f, 1.0f,	// bottom face
			1.0f, 1.0f,
			0.0f, 0.0f,
			1.0f, 0.0f,
			0.0f, 0.0f,	// front face
			1.0f, 0.0f,
			0.0f, 1.0f,
			1.0f, 1.0f,
			1.0f, 0.0f,	// back face
			0.0f, 0.0f,
			1.0f, 1.0f,
			0.0f, 1.0f,
			0.0f, 0.0f,	// left face
			1.0f, 0.0f,
			0.0f, 1.0f,
			1.0f, 1.0f,
			0.0f, 0.0f,	// right face
			1.0f, 0.0f,
			0.0f, 1.0f,
			1.0f, 1.0f,
	};

	GLushort index_buf[] = {
			2, 3, 0, 3, 0, 1,	// top face
			6, 7, 4, 7, 4, 5,	// bottom face
			8, 9, 10, 9, 10, 11,	// front face
			12, 13, 14, 13, 14, 15,	// back face
			16, 17, 18, 17, 18, 19,	// left face
			20, 21, 22, 21, 22, 23,	// right face
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

	GLuint vbo_texcord;
	glGenBuffers(1, &vbo_texcord);
	glBindBuffer(GL_ARRAY_BUFFER, vbo_texcord);
	glBufferData(GL_ARRAY_BUFFER, sizeof(texcord_buf), texcord_buf, GL_STATIC_DRAW);

	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(GLfloat), (GLvoid *)0);

	/* setup index buffer for index
	 */
	GLuint vbo_index;
	glGenBuffers(1, &vbo_index);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, vbo_index);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(index_buf), index_buf, GL_STATIC_DRAW);

	/* setup shader */
	GLint status;
	GLchar message[512];

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
	glUseProgram(program);

	glGetProgramiv(program, GL_LINK_STATUS, &status);
	if (!status) {
		glGetProgramInfoLog(program, 512, NULL, message);
		printf("%s\n", message);
	}

	GLuint mvp_uni = glGetUniformLocation(program, "mvp");

	glDeleteShader(vertex_shader);
	glDeleteShader(fragment_shader);

	/* loading texture image */
	GLuint textures[1];
	glGenTextures(1, textures);

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, textures[0]);
	int width, height;
	unsigned char *img = SOIL_load_image("materials/v4v_01.jpg", &width, &height, 0, SOIL_LOAD_RGBA);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, img);
	SOIL_free_image_data(img);

	glUniform1i(glGetUniformLocation(program, "v4v_01"), 0);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	while (!glfwWindowShouldClose(win))
	{
		glm::mat4 model_mat = glm::mat4(1.0f);

		glm::mat4 view_mat = glm::lookAt(
				glm::vec3(-2, 2, 4),
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
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_SHORT, (void*)0);

		glfwSwapBuffers(win);
	}

	glDeleteTextures(1, textures);
	glDeleteBuffers(1, &vbo_pos);
	glDeleteBuffers(1, &vbo_texcord);
	glDeleteBuffers(1, &vbo_index);
	glDeleteVertexArrays(1, &vao);

	glfwTerminate();

	return 0;
}
