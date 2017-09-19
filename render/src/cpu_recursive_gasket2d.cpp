#include "glrunner.h"
#include <vector>
#include <glm/glm.hpp>

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

void GenerateMesh(glm::vec2 p0, glm::vec2 p1, glm::vec2 p2, int k)
{
	if (k > 0) {
		glm::vec2 m0 = 0.5f*(p0 + p1);
		glm::vec2 m1 = 0.5f*(p1 + p2);
		glm::vec2 m2 = 0.5f*(p2 + p0);

		GenerateMesh(p0, m0, m2, k-1);	// up-tri
		GenerateMesh(m0, p1, m1, k-1); // left-tri
		GenerateMesh(m2, m1, p2, k-1); // right-tri

	} else {
		pVertex.push_back(p0[0]);
		pVertex.push_back(p0[1]);
		pVertex.push_back(p1[0]);
		pVertex.push_back(p1[1]);
		pVertex.push_back(p2[0]);
		pVertex.push_back(p2[1]);

		g_num += 6;
	}
}

int main()
{
	GlRunner *runner = new GlRunner(RenderCB);

	GLuint VS = runner->BuildShaderProgram("shaders/simple.vert", GL_VERTEX_SHADER);
	GLuint FS = runner->BuildShaderProgram("shaders/simple.frag", GL_FRAGMENT_SHADER);
	GLuint PPO = runner->BuildProgramPipeline();

	glProgramUniform4f(FS, glGetUniformLocation(FS, "uColor"), 0.75, 0.0, 0.0, 0.0);

	glUseProgramStages(PPO, GL_FRAGMENT_SHADER_BIT, FS);
	glUseProgramStages(PPO, GL_VERTEX_SHADER_BIT, VS);

	glm::vec2 p0 = glm::vec2(0.0, 1.0);
	glm::vec2 p1 = glm::vec2(-1.0, -1.0);
	glm::vec2 p2 = glm::vec2(1.0, -1.0);

	GenerateMesh(p0, p1, p2, 10);

	std::cout << "TOTAL MESH: " << g_num << std::endl;

	/* loading input attribute */
	GLuint VAO, VBO;
	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &VBO);

	glBindVertexArray(VAO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat)*pVertex.size(), &pVertex[0], GL_STATIC_DRAW);

	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2*sizeof(GLfloat), (GLvoid *)0);

	runner->OnRender();

	return 0;
}
