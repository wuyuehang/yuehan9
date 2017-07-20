#include "glrunner.h"

#define separate_per_vertex_data 1

void RenderCB(GlRunner *runner)
{
	glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
	glClearDepth(1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glDrawArrays(GL_TRIANGLES, 0, 3);
}

int main()
{
	GlRunner *runner = new GlRunner(RenderCB);

#if separate_per_vertex_data
	GLfloat pos_buf[] = {
		0.0, 0.5,
		-0.5, -0.5,
		0.5, -0.5
	};

	GLfloat color_buf[] = {
		1.0, 1.0, 0.0,
		0.0, 1.0, 1.0,
		1.0, 0.0, 1.0
	};

	enum { attr_pos, attr_color, attr_total };

	GLuint vbo[attr_total];
	glGenBuffers(attr_total, vbo);

	glBindBuffer(GL_ARRAY_BUFFER, vbo[attr_pos]);
	glBufferData(GL_ARRAY_BUFFER, sizeof(pos_buf), pos_buf, GL_STATIC_DRAW);

	glBindBuffer(GL_ARRAY_BUFFER, vbo[attr_color]);
	glBufferData(GL_ARRAY_BUFFER, sizeof(color_buf), color_buf, GL_STATIC_DRAW);

	GLuint vao;
	glGenVertexArrays(1, &vao);
	glBindVertexArray(vao);

	glEnableVertexAttribArray(attr_pos);
	glEnableVertexAttribArray(attr_color);

	glVertexAttribFormat(attr_pos, 2, GL_FLOAT, GL_FALSE, 0);
	glVertexAttribFormat(attr_color, 3, GL_FLOAT, GL_FALSE, 0);

	glVertexAttribBinding(attr_pos, 0);
	glVertexAttribBinding(attr_color, 1);

	glBindVertexBuffer(0, vbo[attr_pos], 0, 2 * sizeof(GLfloat));
	glBindVertexBuffer(1, vbo[attr_color], 0, 3 * sizeof(GLfloat));

#else
	GLfloat interleave[] = {
		0.0, 0.5, 1.0, 0.0, 0.0,
		-0.5, -0.5, 0.0, 1.0, 0.0,
		0.5, -0.5, 0.0, 0.0, 1.0
	};

	enum { attr_pos, attr_color };

	GLuint vbo;
	glGenBuffers(1, &vbo);
	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	glBufferData(GL_ARRAY_BUFFER, sizeof(interleave), interleave, GL_STATIC_DRAW);

	GLuint vao;
	glGenVertexArrays(1, &vao);
	glBindVertexArray(vao);

	glEnableVertexAttribArray(attr_pos);
	glEnableVertexAttribArray(attr_color);

	glVertexAttribFormat(attr_pos, 2, GL_FLOAT, GL_FALSE, 0);
	glVertexAttribFormat(attr_color, 3, GL_FLOAT, GL_FALSE, 2 * sizeof(GLfloat));

	glVertexAttribBinding(attr_pos, 0);
	glVertexAttribBinding(attr_color, 0);

	glBindVertexBuffer(0, vbo, 0, 5 * sizeof(GLfloat));
#endif

	GLuint vsProgram = runner->BuildShaderProgram("shaders/attrib2.vert", GL_VERTEX_SHADER);

	GLuint fsProgram = runner->BuildShaderProgram("shaders/attrib2.frag", GL_FRAGMENT_SHADER);

	GLuint ppline = runner->BuildProgramPipeline();

	glUseProgramStages(ppline, GL_VERTEX_SHADER_BIT, vsProgram);
	glUseProgramStages(ppline, GL_FRAGMENT_SHADER_BIT, fsProgram);

	runner->OnRender();

	return 0;
}
