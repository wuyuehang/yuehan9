#define GL_GLEXT_PROTOTYPES

#include <GL/glut.h>
#include <GL/gl.h>
#include <GL/glext.h>

GLuint g_vbo;
GLuint g_program;

static const char *vs_src[] = {"#version 430\n"
  "layout (location = 0) in vec3 Position;"
  "void main(){"
  "gl_Position = vec4(Position.x, Position.y, Position.z, 1.0);}"};

static const char *ps_src[] = {"#version 430\n"
  "out vec4 color;"
  "void main(){"
  "color = vec4(1.0, 0.0, 0.0, 1.0);}"};

void compile_shader()
{
  g_program = glCreateProgram();

  GLuint vs_shader = glCreateShader(GL_VERTEX_SHADER);
  glShaderSource(vs_shader, 1, vs_src, NULL);
  glCompileShader(vs_shader);

  GLuint ps_shader = glCreateShader(GL_FRAGMENT_SHADER);
  glShaderSource(ps_shader, 1, ps_src, NULL);
  glCompileShader(ps_shader);

  glAttachShader(g_program, vs_shader);
  glAttachShader(g_program, ps_shader);

  glLinkProgram(g_program);

  glUseProgram(g_program);
}

void keycb(unsigned char key, int x, int y)
{
  if (key == 'q') exit(0);
}

void render()
{
  glClear(GL_COLOR_BUFFER_BIT);

  glEnableVertexAttribArray(0);

  glBindBuffer(GL_ARRAY_BUFFER, g_vbo);

  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);

  glDrawArrays(GL_TRIANGLES, 0, 3);

  glDisableVertexAttribArray(0);

  glutSwapBuffers();
}

void resource()
{
  GLfloat position_vb[3][3] = {{-0.5f, -0.5f, 0.0f}, {0.5f, -0.5f, 0.0f}, {0.0f, 0.5f, 0.0f}};

  glGenBuffers(1, &g_vbo);

  glBindBuffer(GL_ARRAY_BUFFER, g_vbo);

  glBufferData(GL_ARRAY_BUFFER, sizeof(position_vb), position_vb, GL_STATIC_DRAW);
}

int main(int argc, char**argv)
{
  glutInit(&argc, argv);

  glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA);

  glutInitWindowSize(500, 500);

  glutCreateWindow("render");

  glutKeyboardFunc(keycb);

  glutDisplayFunc(render);

  glClearColor(0.0f, 0.0f, 0.0f, 0.0f);

  resource();

  compile_shader();

  glutMainLoop();

  return 0;
}
