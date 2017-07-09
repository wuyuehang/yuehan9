#include <GL/glew.h>
#include <GL/freeglut.h>
#include <glm/glm.hpp>
#include <glm/gtx/transform.hpp>
#include <glm/gtx/rotate_vector.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include "warp.h"

#define WIDTH	1280
#define HEIGHT 1024

GLuint Prog;
int Window;

// camera roaming
float Camera_StepRate = 0.25;
glm::vec3 RoamCameraLoc = glm::vec3(0, 0, 2);
glm::vec3 RoamLensDirect = glm::vec3(0, 0, -1);
glm::vec3 RoamCameraUp = glm::vec3(0, 1, 0);

void dbg_callback(GLenum source, GLenum type, GLuint id,
		GLenum severity, GLsizei lenghth,
		const GLchar *message, const void *userParam)
{
	printf("(%d): %s\n", id, message);
}

void NormalKeyCB(unsigned char key, int x, int y)
{
	switch (key)
	{
		case 27:	// Escape
			glutDestroyWindow(Window);
			break;
		case 'w':
			RoamCameraLoc += RoamLensDirect * Camera_StepRate;
			break;
		case 's':
			RoamCameraLoc -= RoamLensDirect * Camera_StepRate;
			break;
		case 'a':
			RoamCameraLoc += glm::normalize(glm::cross(RoamCameraUp, RoamLensDirect)) * Camera_StepRate;
			break;
		case 'd':
			RoamCameraLoc -= glm::normalize(glm::cross(RoamCameraUp, RoamLensDirect)) * Camera_StepRate;
			break;
	}

	printf("Camera Pos %f, %f, %f\n", RoamCameraLoc[0],
			RoamCameraLoc[1], RoamCameraLoc[2]);

	printf("Camera Target %f, %f, %f\n", RoamLensDirect[0],
			RoamLensDirect[1], RoamLensDirect[2]);

	return;
}

void RenderCB()
{
	glClearColor(0.2, 0.05, 0.1, 1.0);
	glClearDepthf(1.0);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glm::mat4 Model = glm::mat4(1.0);

#if 0
	// look from +z to -z direction
	glm::vec3 CameraLoc = glm::vec3(0, 0, 2);
	glm::vec3 LensDirect = glm::vec3(0, 0, 0);
	glm::mat4 View = glm::lookAt(
			CameraLoc,
			LensDirect,
			glm::vec3(0, 1, 0)
	);
#endif

	glm::mat4 View = glm::lookAt(
			RoamCameraLoc, RoamLensDirect, RoamCameraUp);

	float FOV = 45.0;
	float Ratio = 1.0;
	float Near = 0.01;
	float Far = 100.0;

	glm::mat4 Projection = glm::perspective(glm::radians(FOV), Ratio, Near, Far);

	// update uniform per frame
	glUniformMatrix4fv(glGetUniformLocation(Prog, "uModel"), 1, GL_FALSE, &Model[0][0]);
	glUniformMatrix4fv(glGetUniformLocation(Prog, "uView"), 1, GL_FALSE, &View[0][0]);
	glUniformMatrix4fv(glGetUniformLocation(Prog, "uProjection"), 1, GL_FALSE, &Projection[0][0]);
	glDrawArrays(GL_TRIANGLES, 0, 6);
	glutSwapBuffers();
}

int main(int argc, char **argv)
{
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA | GLUT_DEPTH);
	glutInitContextVersion(4, 5);
	glutInitContextFlags(GLUT_FORWARD_COMPATIBLE);
	glutInitContextProfile(GLUT_CORE_PROFILE);
	glutInitWindowSize(WIDTH, HEIGHT);
	Window = glutCreateWindow(__FILE__);

	glewExperimental = true;
	assert(GLEW_OK == glewInit());
	glutKeyboardFunc(NormalKeyCB);
	glutDisplayFunc(RenderCB);
	glutIdleFunc(RenderCB);

	// preamble
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LESS);
	glEnable(GL_DEBUG_OUTPUT);
	glDebugMessageCallback(dbg_callback, NULL);
	glDebugMessageControl(GL_DONT_CARE, GL_DONT_CARE,
			GL_DONT_CARE, 0, NULL, GL_TRUE);

	glViewport(0, 0, WIDTH, HEIGHT);

	GLuint VAO;
	GLuint VBO[2];
	glGenVertexArrays(1, &VAO);
	glBindVertexArray(VAO);

	GLfloat pos[] = {
		0.0, 0.5, 0.0,
		-0.5, -0.5, 0.0,
		0.5, -0.5, 0.0,
		// since camera looks into -z direction, the bigger z maps to near plane
		0.0, 0.0, 0.1,
		0.0, 0.3, 0.1,
		0.3, 0.0, 0.1
	};

	GLfloat color[] = {
		1.0, 1.0, 0.0,
		1.0, 1.0, 0.0,
		1.0, 1.0, 0.0,
		0.0, 1.0, 1.0,
		0.0, 1.0, 1.0,
		0.0, 1.0, 1.0
	};

	glGenBuffers(2, VBO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO[0]);
	glBufferData(GL_ARRAY_BUFFER, sizeof(pos), pos, GL_STATIC_DRAW);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3*sizeof(GLfloat), (GLvoid *)0);

	glBindBuffer(GL_ARRAY_BUFFER, VBO[1]);
	glBufferData(GL_ARRAY_BUFFER, sizeof(color), color, GL_STATIC_DRAW);
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 3*sizeof(GLfloat), (GLvoid *)0);

	// shader and program
	GLuint VS = BuildShader("shaders/simple.vert", GL_VERTEX_SHADER);
	GLuint FS = BuildShader("shaders/simple.frag", GL_FRAGMENT_SHADER);
	Prog = BuildProgram(VS, FS);

	glutMainLoop();

	return 0;
}
