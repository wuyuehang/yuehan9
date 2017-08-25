#include "glrunner.h"
#include <cstring>

#define _accumulate_	1

GLuint atcVertice;
GLuint atcPrimitive;

void RenderCB(GlRunner *runner)
{
	glClearColor(0.0, 0.0, 0.0, 1.0);
	glClearDepthf(1.0);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glDrawArrays(GL_POINTS, 0, 3);

	// read back how many vertices being processed
	glBindBuffer(GL_ATOMIC_COUNTER_BUFFER, atcVertice);
	GLuint *userCount;
	userCount = (GLuint *)glMapBufferRange(GL_ATOMIC_COUNTER_BUFFER,
			0, sizeof(GLuint), GL_MAP_WRITE_BIT | GL_MAP_READ_BIT);

	std::cout << "vertices processing " << *userCount << std::endl;

#if !_accumulate_
	memset(userCount, 0, sizeof(GLuint));
#endif
	glUnmapBuffer(GL_ATOMIC_COUNTER_BUFFER);

	// read back how many primitives being processed
	glBindBuffer(GL_ATOMIC_COUNTER_BUFFER, atcPrimitive);
	userCount = (GLuint *)glMapBufferRange(GL_ATOMIC_COUNTER_BUFFER,
			0, sizeof(GLuint), GL_MAP_WRITE_BIT | GL_MAP_READ_BIT);

	std::cout << "primitives processing " << *userCount << std::endl;

#if !_accumulate_
	memset(userCount, 0, sizeof(GLuint));
#endif
	glUnmapBuffer(GL_ATOMIC_COUNTER_BUFFER);

}

int main()
{
	GlRunner *runner = new GlRunner(RenderCB);

	GLfloat pos[] = {
			-0.5f, -0.5f,
			 0.5f, -0.5f,
			 0.0f,  0.5f
	};

	GLfloat color[] = {
			1.0f, 0.0f, 0.0f,
			0.0f, 1.0f, 0.0f,
			0.0f, 0.0f, 1.0f
	};

	GLuint vao, vbo[2];
	glGenVertexArrays(1, &vao);
	glBindVertexArray(vao);

	glGenBuffers(2, vbo);
	glBindBuffer(GL_ARRAY_BUFFER, vbo[0]);
	glBufferData(GL_ARRAY_BUFFER, sizeof(pos), pos, GL_STATIC_DRAW);

	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(GLfloat), (GLvoid*)0);
	glEnableVertexAttribArray(0);

	glBindBuffer(GL_ARRAY_BUFFER, vbo[1]);
	glBufferData(GL_ARRAY_BUFFER, sizeof(color), color, GL_STATIC_DRAW);

	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), (GLvoid*)0);
	glEnableVertexAttribArray(1);

	// buffer storage for vertices atomic counter
	glGenBuffers(1, &atcVertice);
	glBindBuffer(GL_ATOMIC_COUNTER_BUFFER, atcVertice);
	glBufferData(GL_ATOMIC_COUNTER_BUFFER, sizeof(GLuint), nullptr, GL_DYNAMIC_DRAW);

	// populate binding point for atomic counter
	// vertice counter binding in index 0 and offset 0;
	glBindBufferBase(GL_ATOMIC_COUNTER_BUFFER, 0, atcVertice);

	// reset atomic counter to zero
	GLuint *userCount;
	userCount = (GLuint *)glMapBufferRange(GL_ATOMIC_COUNTER_BUFFER,
			0, sizeof(GLuint), GL_MAP_WRITE_BIT | GL_MAP_READ_BIT);
	memset(userCount, 0, sizeof(GLuint));
	glUnmapBuffer(GL_ATOMIC_COUNTER_BUFFER);

	// buffer storage for primitives atomic counter
	glGenBuffers(1, &atcPrimitive);
	glBindBuffer(GL_ATOMIC_COUNTER_BUFFER, atcPrimitive);
	glBufferData(GL_ATOMIC_COUNTER_BUFFER, sizeof(GLuint), nullptr, GL_DYNAMIC_DRAW);

	// populate binding point for atomic counter
	// vertice counter binding in index 1 and offset 0;
	glBindBufferBase(GL_ATOMIC_COUNTER_BUFFER, 1, atcPrimitive);

	// reset atomic counter to zero
	userCount = (GLuint *)glMapBufferRange(GL_ATOMIC_COUNTER_BUFFER,
			0, sizeof(GLuint), GL_MAP_WRITE_BIT | GL_MAP_READ_BIT);
	memset(userCount, 0, sizeof(GLuint));
	glUnmapBuffer(GL_ATOMIC_COUNTER_BUFFER);

	GLuint vsProgram = runner->BuildShaderProgram("shaders/atomic_count_vertice.vert", GL_VERTEX_SHADER);

	GLuint gsProgram = runner->BuildShaderProgram("shaders/atomic_count_triangle.geo", GL_GEOMETRY_SHADER);

	GLuint fsProgram = runner->BuildShaderProgram("shaders/attrib2.frag", GL_FRAGMENT_SHADER);

	GLuint ppline = runner->BuildProgramPipeline();

	glUseProgramStages(ppline, GL_VERTEX_SHADER_BIT, vsProgram);

	glUseProgramStages(ppline, GL_GEOMETRY_SHADER_BIT, gsProgram);

	glUseProgramStages(ppline, GL_FRAGMENT_SHADER_BIT, fsProgram);

	runner->OnRender();

	return 0;
}
