#include "glrunner.h"
#include <vector>
#include <glm/glm.hpp>
#include <glm/gtx/transform.hpp>
#include <glm/gtx/rotate_vector.hpp>
#include <glm/gtc/matrix_transform.hpp>

using std::vector;

vector<GLfloat> pVertex;
unsigned int g_num = 0;

void RenderCB(GlRunner *runner)
{
	glClearColor(0.15, 0.05, 0.15, 0.0);
	glClearDepthf(1.0);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glDrawArrays(GL_TRIANGLES, 0, g_num);
}

void GenerateMesh(glm::vec3 p0, glm::vec3 p1, glm::vec3 p2, glm::vec3 p3, int k)
{
	if (k > 0) {

		glm::vec3 m01 = 0.5f*(p0 + p1);
		glm::vec3 m12 = 0.5f*(p1 + p2);
		glm::vec3 m13 = 0.5f*(p1 + p3);

		glm::vec3 m02 = 0.5f*(p0 + p2);
		glm::vec3 m03 = 0.5f*(p0 + p3);
		glm::vec3 m23 = 0.5f*(p3 + p2);

		// left pyramid
		GenerateMesh(m01, p1, m12, m13, k-1);

		// middle pyramid
		GenerateMesh(m02, m12, p2, m23, k-1);

		// right pyramid
		GenerateMesh(m03, m13, m23, p3, k-1);

		// up pyramid
		GenerateMesh(p0, m01, m02, m03, k-1);

	} else {
		// s012
		pVertex.push_back(p0[0]);
		pVertex.push_back(p0[1]);
		pVertex.push_back(p0[2]);

		pVertex.push_back(p1[0]);
		pVertex.push_back(p1[1]);
		pVertex.push_back(p1[2]);

		pVertex.push_back(p2[0]);
		pVertex.push_back(p2[1]);
		pVertex.push_back(p2[2]);

		// s023
		pVertex.push_back(p0[0]);
		pVertex.push_back(p0[1]);
		pVertex.push_back(p0[2]);

		pVertex.push_back(p2[0]);
		pVertex.push_back(p2[1]);
		pVertex.push_back(p2[2]);

		pVertex.push_back(p3[0]);
		pVertex.push_back(p3[1]);
		pVertex.push_back(p3[2]);

		// s213
		pVertex.push_back(p2[0]);
		pVertex.push_back(p2[1]);
		pVertex.push_back(p2[2]);

		pVertex.push_back(p1[0]);
		pVertex.push_back(p1[1]);
		pVertex.push_back(p1[2]);

		pVertex.push_back(p3[0]);
		pVertex.push_back(p3[1]);
		pVertex.push_back(p3[2]);

		// s031
		pVertex.push_back(p0[0]);
		pVertex.push_back(p0[1]);
		pVertex.push_back(p0[2]);

		pVertex.push_back(p3[0]);
		pVertex.push_back(p3[1]);
		pVertex.push_back(p3[2]);

		pVertex.push_back(p1[0]);
		pVertex.push_back(p1[1]);
		pVertex.push_back(p1[2]);

		g_num += 12;
	}
}

int main()
{
	GlRunner *runner = new GlRunner(RenderCB);

	glEnable(GL_CULL_FACE);
	glCullFace(GL_BACK);
	glFrontFace(GL_CCW);
	glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

	GLuint VS = runner->BuildShaderProgram("shaders/cpu_recursive_gasket3d.vert", GL_VERTEX_SHADER);
	GLuint FS = runner->BuildShaderProgram("shaders/simple.frag", GL_FRAGMENT_SHADER);
	GLuint PPO = runner->BuildProgramPipeline();

	glProgramUniform4f(FS, glGetUniformLocation(FS, "uColor"), 0.75, 0.0, 0.0, 0.0);

	glUseProgramStages(PPO, GL_FRAGMENT_SHADER_BIT, FS);
	glUseProgramStages(PPO, GL_VERTEX_SHADER_BIT, VS);

	glm::vec3 p0 = glm::vec3(0.0, 1.0, 0.0);
	glm::vec3 p1 = glm::vec3(-1.5, -0.5, -1.5);
	glm::vec3 p2 = glm::vec3(0.0, -0.5, 1.0);
	glm::vec3 p3 = glm::vec3(1.5, -0.5, -1.5);

	GenerateMesh(p0, p1, p2, p3, 5);

	std::cout << "TOTAL MESH: " << g_num << std::endl;

	/* loading input attribute */
	GLuint VAO, VBO;
	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &VBO);

	glBindVertexArray(VAO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat)*pVertex.size(), &pVertex[0], GL_STATIC_DRAW);

	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3*sizeof(GLfloat), (GLvoid *)0);

	/* setup matrixs */
	glm::mat4 model_mat = glm::scale(glm::vec3(2.0));
	glm::mat4 proj_mat = glm::perspective(glm::radians(45.0f), 1.0f, 0.01f, 100.0f);

	glm::vec3 camera_location(0, 1, 5);

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
