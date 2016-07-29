#include <GL/glut.h>

void displayCB(void)		/* function called whenever redisplay needed */
{
	glClear(GL_COLOR_BUFFER_BIT);		/* clear the display */
	glColor3f(1.0, 1.0, 1.0);		/* set current color to white */
	glPointSize(2.5);
	glBegin(GL_POINTS);
	glColor3f(1.0, 0.0, 0.0);
	glVertex2i(300,200);
	glColor3f(0.0, 1.0, 0.0);
	glVertex2i(400,400);
	glColor3f(0.0, 0.0, 1.0);
	glVertex2i(200,400);
	glEnd();
	glFlush();
}

void keyCB(unsigned char key, int x, int y)
{
	if(key == 'q') exit(0);
}


int main(int argc, char *argv[])
{
	int win;

	glutInit(&argc, argv);		/* initialize GLUT system */

	glutInitDisplayMode(GLUT_RGB);
	glutInitWindowSize(600, 600);
	win = glutCreateWindow("Vertex");

	/* from this point on the current window is win */

	glClearColor(0.0,0.0,0.0,0.0);
	gluOrtho2D(0, 600, 0, 600);
	glutDisplayFunc(displayCB);
	glutKeyboardFunc(keyCB);

	glutMainLoop();

	return 0;
}
