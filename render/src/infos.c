#define GL_GLEXT_PROTOTYPES

#include <stdio.h>
#include <GL/gl.h>
#include <GL/glut.h>
#include <GL/glext.h>

int g_win;

void keyCB(unsigned char key, int x, int y)
{
	if (key == 'q') {
		glutDestroyWindow(g_win);
		exit(0);
	}
}

int main(int argc, char *argv[])
{
	GLuint i;
	GLuint num;

	glutInit(&argc, argv);		/* initialize GLUT system */
	glutInitDisplayMode(GLUT_RGB);
	glutInitWindowSize(600, 600);
	g_win = glutCreateWindow("infos");
	glutKeyboardFunc(keyCB);

	printf("vendor: %s\nrender: %s\nversion: %s\nglsl version: %s\n", glGetString(GL_VENDOR),
			glGetString(GL_RENDER), glGetString(GL_VERSION), glGetString(GL_SHADING_LANGUAGE_VERSION));

	glGetIntegerv(GL_NUM_EXTENSIONS, &num);
	printf("extensions: (%d)\n", num);

	for (i = 0; i < num; i++)
		printf("%s\n", glGetStringi(GL_EXTENSIONS, i));

	glutMainLoop();

	return 0;
}
