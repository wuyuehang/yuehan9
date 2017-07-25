#include "glrunner.h"

#define _map_xfb_for_validation_ 0
#define _query_xfb_written_ 0
#define _xfb_sync_ 1

void RenderCB(GlRunner *)
{
	glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
	glClearDepth(1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glDrawArrays(GL_TRIANGLES, 0, 3);
}

int main()
{
	GlRunner *runner = new GlRunner(RenderCB);

	glEnable(GL_RASTERIZER_DISCARD);
	// create xfb render pass, translate the original
	// vertice to up-right by one unit
	// can't use separate shader object for transform feedback
	// because the output from vertex shader will be optimized out
	const GLchar *xfb_list[] = {"xfb_pos"};
	runner->BuildXfb("shaders/xfb.vert", 1, xfb_list, GL_INTERLEAVED_ATTRIBS);

	GLfloat vb_pos[] = {
		-1.0, 0.0,
		-1.0, -1.0,
		0.0, -1.0,
		0.0, 0.0
	};

	GLuint VAO[2];
	glGenVertexArrays(2, VAO);

	GLuint VBO[2];
	glGenBuffers(2, VBO);

	glBindVertexArray(VAO[0]);
	glBindBuffer(GL_ARRAY_BUFFER, VBO[0]);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vb_pos), vb_pos, GL_STATIC_DRAW);

	glEnableVertexAttribArray(0);
	glVertexAttribFormat(0, 2, GL_FLOAT, GL_FALSE, 0);
	glVertexAttribBinding(0, 0);
	glBindVertexBuffer(0, VBO[0], 0, 2*sizeof(GLfloat));

	// store the transform feedback
	glBindBuffer(GL_ARRAY_BUFFER, VBO[1]);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vb_pos), nullptr, GL_STATIC_READ);
	glBindBufferBase(GL_TRANSFORM_FEEDBACK_BUFFER, 0, VBO[1]);

#if _query_xfb_written_
	GLuint xfb_qry;
	glGenQueries(1, &xfb_qry);

	glBeginQuery(GL_TRANSFORM_FEEDBACK_PRIMITIVES_WRITTEN, xfb_qry);
#endif

	glBeginTransformFeedback(GL_POINTS);
	glDrawArrays(GL_POINTS, 0, 4);
	glEndTransformFeedback();

#if _xfb_sync_
	GLsync xfb_sync = glFenceSync(GL_SYNC_GPU_COMMANDS_COMPLETE, 0);
#endif

#if _query_xfb_written_
	glEndQuery(GL_TRANSFORM_FEEDBACK_PRIMITIVES_WRITTEN);

	GLuint num_primitives;
	glGetQueryObjectuiv(xfb_qry, GL_QUERY_RESULT, &num_primitives);
	std::cout << "xfb writes primitives " << num_primitives << std::endl;
#endif

#if _map_xfb_for_validation_
	GLfloat result[8];
	glGetBufferSubData(GL_TRANSFORM_FEEDBACK_BUFFER, 0, sizeof(result), result);
	int i = 0;
	for (; i < 4; i++) {
		std::cout << "vertice " << i << " x(" << result[2*i] << ") y(" << result[2*i+1] << std::endl;
	}
#endif

	// draw the xfb vertex
	GLuint VS1 = runner->BuildShaderProgram("shaders/simple.vert", GL_VERTEX_SHADER);
	GLuint FS1 = runner->BuildShaderProgram("shaders/simple.frag", GL_FRAGMENT_SHADER);

	GLuint pipe = runner->BuildProgramPipeline();

	glUseProgramStages(pipe, GL_VERTEX_SHADER_BIT, VS1);
	glUseProgramStages(pipe, GL_FRAGMENT_SHADER_BIT, FS1);

	glProgramUniform4f(FS1, glGetUniformLocation(FS1, "uColor"), 1.0, 0.0, 0.0, 1.0);

	glBindVertexArray(VAO[1]);
	glBindBuffer(GL_ARRAY_BUFFER, VBO[1]);
	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(GLfloat), (GLvoid *)0);
	glEnableVertexAttribArray(0);

	glDisable(GL_RASTERIZER_DISCARD);

#if _xfb_sync_
	glWaitSync(xfb_sync, 0, GL_TIMEOUT_IGNORED);
#endif

	runner->OnRender();

	return 0;
}
