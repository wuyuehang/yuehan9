#ifndef __OGL_HELPER_H__
#define __OGL_HELPER_H__

#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <GL/glew.h>
#include <GLFW/glfw3.h>

void load_shader_from_file(const char *filepath, char **content);
void error_cb(int error, const char* description);
void key_cb(GLFWwindow* win, int key, int scancode, int action, int mode);

#endif
