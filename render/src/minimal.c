#define GL_GLEXT_PROTOTYPES

#include <GL/glut.h>
#include <GL/gl.h>
#include <GL/glext.h>
#include <math.h>
#include <assert.h>

/* handle of internal opengl object */
GLuint g_vbo;
GLuint g_program;
GLuint g_scalex;
GLuint g_scaley;
GLuint g_world_mat;
int g_win;

static const char *vs_src[] = {"#version 430\n"
  "layout (location = 0) in vec3 Position;"
  "uniform float scalex;"
  "uniform float scaley;"
  "uniform mat4 world;"
  "out vec4 color;"
  "void main(){"
  "gl_Position = world * vec4(scalex * Position.x, scaley * Position.y, Position.z, 1.0);"
  "color = vec4(clamp(Position, 0.0, 1.0), 1.0);}"};

static const char *ps_src[] = {"#version 430\n"
  "in vec4 color;"
  "out vec4 final_color;"
  "void main(){"
  "final_color = color;}"};

void compile_shader()
{
  g_program = glCreateProgram();
  assert(g_program != 0);

  GLuint vs_shader = glCreateShader(GL_VERTEX_SHADER);
  assert(vs_shader != 0);
  glShaderSource(vs_shader, 1, vs_src, NULL);
  glCompileShader(vs_shader);

  GLuint ps_shader = glCreateShader(GL_FRAGMENT_SHADER);
  assert(ps_shader != 0);
  glShaderSource(ps_shader, 1, ps_src, NULL);
  glCompileShader(ps_shader);

  glAttachShader(g_program, vs_shader);
  glAttachShader(g_program, ps_shader);

  glLinkProgram(g_program);

  glUseProgram(g_program);

  g_scalex = glGetUniformLocation(g_program, "scalex");
  assert(g_scalex != 0xFFFFFFFF);
  g_scaley = glGetUniformLocation(g_program, "scaley");
  assert(g_scaley != 0xFFFFFFFF);
  g_world_mat = glGetUniformLocation(g_program, "world");
  assert(g_world_mat != 0xFFFFFFFF);
}

void keycb(unsigned char key, int x, int y)
{
  if (key == 'q') {
		glutDestroyWindow(g_win);
		exit(0);
	}
}

void render()
{
  static float scale = 0.0;

  scale += 0.002;

  glUniform1f(g_scalex, sinf(scale));
  glUniform1f(g_scaley, cosf(scale));

  GLfloat world[4][4] = {{1.0f, 0.0f, 0.0f, sinf(scale)},
    {0.0f, 1.0f, 0.0f, cosf(scale)},
    {0.0f, 0.0f, 1.0f, 0.0f},
    {0.0f, 0.0f, 0.0f, 1.0f}};

  glUniformMatrix4fv(g_world_mat, 1, GL_TRUE, &world[0][0]);

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
  assert(g_vbo != 0xFFFFFFFF);

  glBindBuffer(GL_ARRAY_BUFFER, g_vbo);

  glBufferData(GL_ARRAY_BUFFER, sizeof(position_vb), position_vb, GL_STATIC_DRAW);
}

int main(int argc, char**argv)
{
  glutInit(&argc, argv);

  glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA);

  glutInitWindowSize(500, 500);

  g_win = glutCreateWindow("render");

  glutKeyboardFunc(keycb);

  glutIdleFunc(render);

  glClearColor(0.0f, 0.0f, 0.0f, 0.0f);

  resource();

  compile_shader();

  glutMainLoop();

  return 0;
}
