#include <GL/glut.h>

static unsigned int style = 0;

void displayCB(void)		/* function called whenever redisplay needed */
{
	glClear(GL_COLOR_BUFFER_BIT);		/* clear the display */
	glLineWidth(2.5);

	switch (style) {
	case 0:
		glBegin(GL_LINES);
		glColor3f(1.0, 0.0, 0.0);
		glVertex2i(300,200);
		glColor3f(0.0, 1.0, 0.0);
		glVertex2i(400,400);

		glVertex2i(400,400);
		glColor3f(0.0, 0.0, 1.0);
		glVertex2i(200,400);

		glVertex2i(200,400);
		glColor3f(1.0, 0.0, 0.0);
		glVertex2i(300,200);
		glEnd();
		break;
	case 1:
		glBegin(GL_LINE_STRIP);
		glColor3f(1.0, 0.0, 0.0);
		glVertex2i(200, 400);
		glColor3f(0.0, 1.0, 0.0);
		glVertex2i(400, 400);

		glVertex2i(400, 400);
		glColor3f(0.0, 0.0, 1.0);
		glVertex2i(200, 200);

		glVertex2i(200, 200);
		glColor3f(0.0, 1.0, 1.0);
		glVertex2i(400, 200);
		glEnd();
		break;
	case 2:
		glBegin(GL_LINE_LOOP);
		glColor3f(1.0, 0.0, 0.0);
		glVertex2i(200, 400);
		glColor3f(0.0, 1.0, 0.0);
		glVertex2i(400, 400);

		glVertex2i(400, 400);
		glColor3f(0.0, 0.0, 1.0);
		glVertex2i(200, 200);

		glVertex2i(200, 200);
		glColor3f(0.0, 1.0, 1.0);
		glVertex2i(400, 200);
		glEnd();
		break;
	}

	glFlush();
}

void keyCB(unsigned char key, int x, int y)
{
	if (key == 'q') exit(0);
	else if (key == ' ') {
		style = (style > 2) ? 0 : (style + 1);
		glutPostRedisplay();
	}
}


int main(int argc, char *argv[])
{
	int win;

	glutInit(&argc, argv);		/* initialize GLUT system */

	glutInitDisplayMode(GLUT_RGB);
	glutInitWindowSize(600, 600);
	win = glutCreateWindow("Line");

	/* from this point on the current window is win */

	glClearColor(0.0,0.0,0.0,0.0);
	gluOrtho2D(0, 600, 0, 600);
	glutDisplayFunc(displayCB);
	glutKeyboardFunc(keyCB);

	glutMainLoop();

	return 0;
}
