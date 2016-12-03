#include <assert.h>
#include <stdio.h>
#include <GL/glew.h>
#include <GLFW/glfw3.h>

extern GLboolean glewExperimental;

static void error_cb(int error, const char* description)
{
	fputs(description, stderr);
	fputs("\n", stderr);
}

void key_cb(GLFWwindow *win, int key, int scancode, int action, int mode)
{
	if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
		glfwSetWindowShouldClose(win, GL_TRUE);
}

int main(int argc, char **argv)
{
	GLFWwindow* win = NULL;
	int width, height;

	glfwSetErrorCallback(error_cb);

	glfwInit();

	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	glfwWindowHint(GLFW_RESIZABLE, GL_FALSE);

	win = glfwCreateWindow(600, 600, "glfw demo", NULL, NULL);
	assert(NULL != win);

	glfwSetKeyCallback(win, key_cb);

	glfwMakeContextCurrent(win);

	glewExperimental = GL_TRUE;
	assert(GLEW_OK == glewInit());

	glfwGetFramebufferSize(win, &width, &height);

	glViewport(0, 0, width, height);

	while(!glfwWindowShouldClose(win)) {
		glfwPollEvents();

		glClearColor(0.2f, 0.3f, 0.4f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT);

		glfwSwapBuffers(win);
	}

	glfwTerminate();

	return 0;
}
