#include <GL/glut.h>

static unsigned int style = 0;
int g_win;

void displayCB(void)		/* function called whenever redisplay needed */
{
	glClear(GL_COLOR_BUFFER_BIT);		/* clear the display */

	switch (style) {
	case 0:
		glColor3f(1.0, 1.0, 1.0);		/* set current color to white */
		glBegin(GL_TRIANGLES);			/* draw filled triangle */
		glVertex2i(300,200);			/* specify each vertex of triangle */
		glVertex2i(400,400);
		glVertex2i(200,400);
		glEnd();				/* OpenGL draws the filled triangle */
		break;
	case 1:
		glBegin(GL_TRIANGLE_STRIP);
		glColor3f(1.0, 1.0, 1.0);
		glVertex2i(100, 100);
		glColor3f(1.0, 0.0, 1.0);
		glVertex2i(180, 500);
		glColor3f(1.0, 0.0, 0.0);
		glVertex2i(290, 480);
		glColor3f(0.0, 1.0, 0.0);
		glVertex2i(390, 370);
		glColor3f(0.0, 0.0, 1.0);
		glVertex2i(480, 130);
		glEnd();
		break;
	case 2:
		glBegin(GL_TRIANGLE_FAN);
		glColor3f(1.0, 1.0, 1.0);
		glVertex2i(100, 100); /* root */

		glColor3f(1.0, 0.0, 1.0);
		glVertex2i(180, 500);
		glColor3f(1.0, 0.0, 0.0);
		glVertex2i(290, 480);
		glColor3f(0.0, 1.0, 0.0);
		glVertex2i(390, 370);
		glColor3f(0.0, 0.0, 1.0);
		glVertex2i(480, 130);
		glEnd();
		break;
	}

	glFlush();				/* Complete any pending operations */
}

void keyCB(unsigned char key, int x, int y)
{
	if (key == 'q') {
		glutDestroyWindow(g_win);
		exit(0);
	} else if (key == ' ') {
		style = (style > 2) ? 0 : (style + 1);
		glutPostRedisplay();
	}
}


int main(int argc, char *argv[])
{
	glutInit(&argc, argv);		/* initialize GLUT system */

	glutInitDisplayMode(GLUT_RGB);
	glutInitWindowSize(600, 600);
	g_win = glutCreateWindow("Triangle");

	/* from this point on the current window is g_win */

	glClearColor(0.0,0.0,0.0,0.0);
	gluOrtho2D(0, 600, 0, 600);
	glutDisplayFunc(displayCB);
	glutKeyboardFunc(keyCB);

	glutMainLoop();

  return 0;
}
