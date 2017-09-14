#include "glrunner.h"
#include <ctime>
#include <glm/glm.hpp>
#include <glm/gtx/transform.hpp>
#include <glm/gtx/rotate_vector.hpp>
#include <glm/gtc/matrix_transform.hpp>

#define ymesh	128
#define xmesh	128

GLuint VS, FS;

clock_t start_time;

void RenderCB(GlRunner *runner)
{
	glClearColor(0.2, 0.1, 0.2, 0.0);
	glClearDepthf(1.0);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	clock_t current_time = clock();
	float delta = 0.00001*(current_time-start_time);

	glProgramUniform1f(VS, glGetUniformLocation(VS, "uTime"), delta);

	glProgramUniform4f(FS, glGetUniformLocation(FS, "uColor"), 1.0, 0.0, 0.0, 0.0);
	glDrawArrays(GL_POINTS, 0, xmesh*ymesh);

	glProgramUniform4f(FS, glGetUniformLocation(FS, "uColor"), 1.0, 1.0, 0.0, 0.0);
	glDrawElements(GL_TRIANGLE_STRIP, xmesh*(ymesh-1)*2, GL_UNSIGNED_INT, (void*)0);
}

GLfloat *GenerateVertexMesh()
{
	float deltax = 2.0 / xmesh;
	float deltay = 2.0 / ymesh;

	GLfloat *blob = new GLfloat[ymesh*xmesh*2];

	for (int i = 0; i < ymesh; i++) {
		for (int j = 0; j < xmesh; j++) {
			blob[i*xmesh*2+j*2] = -1.0 + j*deltax;
			blob[i*xmesh*2+j*2+1] = 1.0 - i*deltay;
		}
	}

	return blob;
}

void PrintVertexMesh(GLfloat *pMesh)
{
	for (int i = 0; i < ymesh; i++) {
		for (int j = 0; j < xmesh; j++) {
			std::cout << "(" << pMesh[i*xmesh*2+j*2] << "," << pMesh[i*xmesh*2+j*2+1] << ")";
		}
		std::cout << std::endl;
	}
}

GLuint *GenerateIndex()
{
	GLuint *index = new GLuint[xmesh*(ymesh-1)*2];

	/* only need line from 0 to n-1 */
	for (int i = 0; i < ymesh-1; i++) {
		for (int j = 0; j < xmesh; j++) {
			if (i%2) {
				index[i*xmesh*2+j*2] = i*xmesh+j;
				index[i*xmesh*2+j*2+1] = (i+1)*xmesh+j;
			} else {
				index[i*xmesh*2+j*2] = i*xmesh+(xmesh-j-1);
				index[i*xmesh*2+j*2+1] = (i+1)*xmesh+(xmesh-j-1);
			}
		}
	}

	return index;
}

int main()
{
	start_time = clock();

	GlRunner *runner = new GlRunner(RenderCB);

	glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

	VS = runner->BuildShaderProgram("shaders/mesh_ripple.vert", GL_VERTEX_SHADER);
	FS = runner->BuildShaderProgram("shaders/mesh_ripple.frag", GL_FRAGMENT_SHADER);
	GLuint PPO = runner->BuildProgramPipeline();
	glUseProgramStages(PPO, GL_VERTEX_SHADER_BIT, VS);
	glUseProgramStages(PPO, GL_FRAGMENT_SHADER_BIT, FS);

	GLfloat *pMesh = GenerateVertexMesh();
	//PrintVertexMesh(pMesh);

	GLuint VAO, VBO[2];
	glGenVertexArrays(1, &VAO);
	glGenBuffers(2, VBO);
	glBindVertexArray(VAO);

	glBindBuffer(GL_ARRAY_BUFFER, VBO[0]);
	glBufferData(GL_ARRAY_BUFFER, 2*sizeof(GLfloat)*xmesh*ymesh, pMesh, GL_STATIC_DRAW);

	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2*sizeof(GLfloat), 0);

	GLuint *pIndex = GenerateIndex();

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, VBO[1]);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, 2*sizeof(GLuint)*xmesh*(ymesh-1), pIndex, GL_STATIC_DRAW);

	glPointSize(4.0);

	/* setup matrixs */
	glm::mat4 model_mat = glm::scale(glm::vec3(3.0));
	glm::mat4 proj_mat = glm::perspective(glm::radians(75.0f), 1.0f, 0.01f, 100.0f);

	glm::vec3 camera_location(0, 2.5, 5);

	glm::mat4 view_mat = glm::lookAt(
			camera_location,
			glm::vec3(0, 0, 0),
			glm::vec3(0, 1, 0)
	);

	glm::mat4 mvp_mat = proj_mat * view_mat * model_mat;

	glProgramUniformMatrix4fv(VS, glGetUniformLocation(VS, "uMVP"), 1, GL_FALSE, &mvp_mat[0][0]);

	runner->OnRender();

	return 0;
}
