#include <stdio.h>
#include <GL/glew.h>
#include <GLFW/glfw3.h>

const GLchar* vs_src = "#version 330 core\n"
		"layout (location = 0) in vec4 position;"
		"layout (location = 1) in vec4 color;"
		"out vec4 frag_color;"
		"out float feedback;"
		"void main()"
		"{"
		"gl_Position = position;"
		"frag_color = color;"
		"feedback = sqrt(abs(position.x));"
		"}";

const GLchar* fs_src = "#version 330 core\n"
		"in vec4 frag_color;"
		"out vec4 color;"
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

	GLfloat vb_pos[] = {
		-0.5f, 0.5f, 0.0f, 1.0f,
		0.5f, 0.5f, 0.0f, 1.0f,
		-0.5f, -0.5f, 0.0f, 1.0f,
		0.5f, -0.5f, 0.0f, 1.0f,
	};

	GLfloat vb_color[] = {
		1.0f, 0.0f, 0.0f, 1.0f,
		0.0f, 1.0f, 0.0f, 1.0f,
		1.0f, 0.0f, 1.0f, 1.0f,
		1.0f, 1.0f, 0.0f, 1.0f,
	};

	GLuint vao;
	glGenVertexArrays(1, &vao);
	glBindVertexArray(vao);

	GLuint vbo_pos;
	glGenBuffers(1, &vbo_pos);
	glBindBuffer(GL_ARRAY_BUFFER, vbo_pos);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vb_pos), vb_pos, GL_STATIC_DRAW);

	GLuint vbo_color;
	glGenBuffers(1, &vbo_color);
	glBindBuffer(GL_ARRAY_BUFFER, vbo_color);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vb_color), vb_color, GL_STATIC_DRAW);

	glBindBuffer(GL_ARRAY_BUFFER, vbo_pos);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 4 * sizeof(GLfloat), (GLvoid *)0);

	glBindBuffer(GL_ARRAY_BUFFER, vbo_color);
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, 4 * sizeof(GLfloat), (GLvoid *)0);

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

	/* insert feed back value */
	GLchar* feedback_list[] = {"feedback"};
	glTransformFeedbackVaryings(program, 1, feedback_list, GL_INTERLEAVED_ATTRIBS);

	glLinkProgram(program);

	glGetProgramiv(program, GL_LINK_STATUS, &status);
	if (!status) {
		glGetProgramInfoLog(program, 512, NULL, message);
		printf("%s\n", message);
	}

	glUseProgram(program);

	glDeleteShader(vertex_shader);
	glDeleteShader(fragment_shader);

	/* create feedback bo */
	GLuint vbo_fb;
	glGenBuffers(1, &vbo_fb);
	glBindBuffer(GL_ARRAY_BUFFER, vbo_fb);
	glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat) * 4, NULL, GL_STATIC_READ);


	glBindBufferBase(GL_TRANSFORM_FEEDBACK_BUFFER, 0, vbo_fb);

	while (!glfwWindowShouldClose(win))
	{
		glfwPollEvents();

		glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
		glClear(GL_COLOR_BUFFER_BIT);

		glBeginTransformFeedback(GL_POINTS);
		glDrawArrays(GL_POINTS, 0, 4);
		glEndTransformFeedback();

		glfwSwapBuffers(win);
	}

	GLfloat result[4];
	glGetBufferSubData(GL_TRANSFORM_FEEDBACK_BUFFER, 0, sizeof(result), result);
	printf("x(%f) y(%f) z(%f) w(%f)\n", result[0], result[1], result[2], result[3]);

	glfwTerminate();

	return 0;
}
