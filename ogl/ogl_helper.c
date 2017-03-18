#include "ogl_helper.h"

void load_shader_from_file(const char *filepath, char **content)
{
	FILE *fp = fopen(filepath, "rb");
	long length = -1;

	if (NULL == fp) {
		printf("filepath: %s doesn't exist\n", filepath);
		return;
	}

	fseek(fp, 0, SEEK_END);
	length = ftell(fp);
	fseek(fp, 0, SEEK_SET);

	*content = (char *)malloc(length + 1);

	if (NULL == *content) {
		printf("out of memory\n");
		goto exit;
	}

	memset(*content, 0 , length +1);

	if (length != fread(*content, 1, length, fp))
		printf("fail to read all content\n");

exit:
	fclose(fp);
}

void error_cb(int error, const char* description)
{
	fputs(description, stderr);
	fputs("\n", stderr);
}

void key_cb(GLFWwindow* win, int key, int scancode, int action, int mode)
{
	if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
		glfwSetWindowShouldClose(win, GL_TRUE);
}
