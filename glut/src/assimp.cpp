#include <GL/glew.h>
#include <GL/freeglut.h>
#include <glm/glm.hpp>
#include <glm/gtx/transform.hpp>
#include <glm/gtx/rotate_vector.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include "warp.h"
#include "mesh_loader.h"

#define WIDTH	1280
#define HEIGHT 1024
#define CameraGrid	float(0.5)

float HorizontalDelta = 0.0;
float VerticalDelta = 0.0;
float FOV = 45.0;
float Ratio = 1.0;
float Near = 0.01;
float Far = 100.0;

GLuint Prog;
int Window;
Mesh *pMeshContainer;

glm::vec3 LightLocation = glm::vec3(0.0, 4.0, 4.0);
// camera roaming
glm::vec3 RoamCameraLoc = glm::vec3(0, 0, 5);
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
			RoamCameraLoc += RoamLensDirect * CameraGrid;
			break;
		case 's':
			RoamCameraLoc -= RoamLensDirect * CameraGrid;
			break;
		case 'a':
			RoamCameraLoc += glm::normalize(glm::cross(RoamCameraUp, RoamLensDirect)) * CameraGrid;
			break;
		case 'd':
			RoamCameraLoc -= glm::normalize(glm::cross(RoamCameraUp, RoamLensDirect)) * CameraGrid;
			break;
		case 'q':
			RoamCameraLoc -= RoamCameraUp * CameraGrid;
			break;
		case 'e':
			RoamCameraLoc += RoamCameraUp * CameraGrid;
			break;
		case ' ':
			RoamCameraLoc = glm::vec3(0, 0, 5);
			RoamLensDirect = glm::vec3(0, 0, -1);
			break;
		case '1':
			static bool wireframe = true;
			if (wireframe) {
				glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
			} else {
				glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
			}
			wireframe = !wireframe;
			break;
		case '2':
			static bool cullface = true;
			if (cullface) {
				glEnable(GL_CULL_FACE);
				glCullFace(GL_BACK);
				glFrontFace(GL_CCW);
			} else {
				glDisable(GL_CULL_FACE);
			}
			cullface = !cullface;
			break;
	}

	return;
}

void SpecialKeyCB(int key, int x, int y)
{
	switch (key)
	{
		case GLUT_KEY_LEFT:
			HorizontalDelta -= 0.05;
			break;
		case GLUT_KEY_RIGHT:
			HorizontalDelta += 0.05;
			break;
		case GLUT_KEY_UP:
			VerticalDelta += 0.05;
			break;
		case GLUT_KEY_DOWN:
			VerticalDelta -= 0.05;
			break;
	}

	return;
}

void RenderCB()
{
	glClearColor(0.2, 0.05, 0.1, 1.0);
	glClearDepthf(1.0);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glm::mat4 Model = glm::mat4(1.0);
	glm::mat4 ModelTranslation = glm::translate(glm::vec3(HorizontalDelta, VerticalDelta, 0));
	Model = ModelTranslation * Model;

	glm::mat4 View = glm::lookAt(
			RoamCameraLoc, RoamLensDirect, RoamCameraUp);

	glm::mat4 Projection = glm::perspective(glm::radians(FOV), Ratio, Near, Far);

	// update uniform per frame
	glUniformMatrix4fv(glGetUniformLocation(Prog, "uModel"), 1, GL_FALSE, &Model[0][0]);
	glUniformMatrix4fv(glGetUniformLocation(Prog, "uView"), 1, GL_FALSE, &View[0][0]);
	glUniformMatrix4fv(glGetUniformLocation(Prog, "uProjection"), 1, GL_FALSE, &Projection[0][0]);

	// install light
	glUniform3fv(glGetUniformLocation(Prog, "uLightLoc"), 1, &LightLocation[0]);

	pMeshContainer->RenderMesh();

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
	glutSpecialFunc(SpecialKeyCB);
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
	glGenVertexArrays(1, &VAO);
	glBindVertexArray(VAO);

	pMeshContainer = new Mesh();

	pMeshContainer->LoadMesh("objs/stormtrooper.obj");

	// shader and program
	GLuint VS = BuildShader("shaders/assimp.vert", GL_VERTEX_SHADER);
	GLuint FS = BuildShader("shaders/assimp.frag", GL_FRAGMENT_SHADER);
	Prog = BuildProgram(VS, FS);

	glutMainLoop();

	return 0;
}
