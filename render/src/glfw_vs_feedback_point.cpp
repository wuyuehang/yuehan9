#include <stdio.h>
#include <GL/glew.h>
#include <GLFW/glfw3.h>

const GLchar* vs_src = "#version 450 core\n"
		"layout (location = 0) in vec4 position;"
		"out float feedback;"
		"void main()"
		"{"
		"gl_Position = position;"
		"feedback = sqrt(abs(position.x));"
		"}";

int main()
{
	glfwInit();

	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 5);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	GLFWwindow* win = glfwCreateWindow(500, 500, __FILE__, NULL, NULL);
	glfwMakeContextCurrent(win);

	glewExperimental = GL_TRUE;
	glewInit();

	glViewport(0, 0, 500, 500);

	GLfloat vb_pos[] = {
		-0.5, 0.5,
		0.5, 0.5,
		-0.5, -0.5,
		0.5, -0.5
	};

	glEnable(GL_RASTERIZER_DISCARD);
	GLuint vao;
	glGenVertexArrays(1, &vao);
	glBindVertexArray(vao);

	GLuint vbo_pos;
	glGenBuffers(1, &vbo_pos);
	glBindBuffer(GL_ARRAY_BUFFER, vbo_pos);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vb_pos), vb_pos, GL_STATIC_DRAW);

	glBindBuffer(GL_ARRAY_BUFFER, vbo_pos);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(GLfloat), (GLvoid *)0);

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

	GLuint program = glCreateProgram();
	glAttachShader(program, vertex_shader);

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

	GLuint vbo_xfb;
	glGenBuffers(1, &vbo_xfb);
	glBindBuffer(GL_TRANSFORM_FEEDBACK_BUFFER, vbo_xfb);
	glBufferData(GL_TRANSFORM_FEEDBACK_BUFFER, sizeof(GLfloat) * 4, NULL, GL_STATIC_READ);

	glBindBufferBase(GL_TRANSFORM_FEEDBACK_BUFFER, 0, vbo_xfb);

	GLuint xfb_qry;
	glGenQueries(1, &xfb_qry);

	glBeginQuery(GL_TRANSFORM_FEEDBACK_PRIMITIVES_WRITTEN, xfb_qry);

	glBeginTransformFeedback(GL_POINTS);
	glDrawArrays(GL_POINTS, 0, 4);
	glEndTransformFeedback();

	glEndQuery(GL_TRANSFORM_FEEDBACK_PRIMITIVES_WRITTEN);

	GLuint num_primitives;
	glGetQueryObjectuiv(xfb_qry, GL_QUERY_RESULT, &num_primitives);
	printf("xfb writes %d primitives\n", num_primitives);

	GLfloat result[4];
	glGetBufferSubData(GL_TRANSFORM_FEEDBACK_BUFFER, 0, sizeof(result), result);
	printf("x(%f) y(%f) z(%f) w(%f)\n", result[0], result[1], result[2], result[3]);

	glfwTerminate();

	return 0;
}
