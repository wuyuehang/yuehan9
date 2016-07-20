#include <GL/glut.h>

void displayCB(void)		/* function called whenever redisplay needed */
{
	glClear(GL_COLOR_BUFFER_BIT);		/* clear the display */
	glColor3f(1.0, 1.0, 1.0);		/* set current color to white */
	glBegin(GL_POLYGON);			/* draw filled triangle */
	glVertex2i(300,200);			/* specify each vertex of triangle */
	glVertex2i(400,400);
	glVertex2i(200,400);
	glEnd();				/* OpenGL draws the filled triangle */
	glFlush();				/* Complete any pending operations */
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
	win = glutCreateWindow("Triangle");

	/* from this point on the current window is win */

	glClearColor(0.0,0.0,0.0,0.0);
	gluOrtho2D(0, 600, 0, 600);
	glutDisplayFunc(displayCB);
	glutKeyboardFunc(keyCB);

	glutMainLoop();

  return 0;
}
