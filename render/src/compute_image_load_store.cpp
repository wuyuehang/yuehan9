#include "glrunner.h"

void RenderCB(GlRunner *runner)
{
	glClearColor(0.0, 0.0, 0.0, 0.0);
	glClearDepthf(1.0);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glDrawArrays(GL_TRIANGLES, 0, 6);
}

int main()
{
	GlRunner *runner = new GlRunner(RenderCB);

	// preamble query compute shader related limitation
	GLint param;
	glGetIntegerv(GL_MAX_COMPUTE_SHADER_STORAGE_BLOCKS, &param);
	std::cout << "######## shader storage blocks " << param << std::endl;
	glGetIntegerv(GL_MAX_COMPUTE_UNIFORM_BLOCKS, &param);
	std::cout << "######## uniform blocks " << param << std::endl;
	glGetIntegerv(GL_MAX_COMPUTE_TEXTURE_IMAGE_UNITS, &param);
	std::cout << "######## texture image units " << param << std::endl;
	glGetIntegerv(GL_MAX_COMPUTE_UNIFORM_COMPONENTS, &param);
	std::cout << "######## uniform components " << param << std::endl;
	glGetIntegerv(GL_MAX_COMPUTE_ATOMIC_COUNTERS, &param);
	std::cout << "######## atomic counters " << param << std::endl;
	glGetIntegerv(GL_MAX_COMPUTE_ATOMIC_COUNTER_BUFFERS, &param);
	std::cout << "######## atomic counter buffers " << param << std::endl;

	// local_size_x * local_size_y * local_size_z
	glGetIntegerv(GL_MAX_COMPUTE_WORK_GROUP_INVOCATIONS, &param);
	std::cout << "######## workgroup invocation(x*y*z) " << param << std::endl;
	GLint work_group_size[3];
	// local work group dimension limitation
	// shader qualifier like local_size_x, local_size_y, local_size_z
	glGetIntegeri_v(GL_MAX_COMPUTE_WORK_GROUP_SIZE, 0, &work_group_size[0]);
	glGetIntegeri_v(GL_MAX_COMPUTE_WORK_GROUP_SIZE, 1, &work_group_size[1]);
	glGetIntegeri_v(GL_MAX_COMPUTE_WORK_GROUP_SIZE, 2, &work_group_size[2]);
	std::cout << "######## workgroup size(x-y-z) " << work_group_size[0] << "-"
		<< work_group_size[1] << "-" << work_group_size[2] << std::endl;

	// dispatch work group dimension limitation
	// in glDispatchCompute's XYZ
	GLint dispatch_wg_count[3];
	glGetIntegeri_v(GL_MAX_COMPUTE_WORK_GROUP_COUNT, 0, &dispatch_wg_count[0]);
	glGetIntegeri_v(GL_MAX_COMPUTE_WORK_GROUP_COUNT, 1, &dispatch_wg_count[1]);
	glGetIntegeri_v(GL_MAX_COMPUTE_WORK_GROUP_COUNT, 2, &dispatch_wg_count[2]);
	std::cout << "######## dispatch workgroup size(X-Y-Z) " << dispatch_wg_count[0]
		<< "-" << dispatch_wg_count[1] << "-" << dispatch_wg_count[2] << std::endl;

	// computing
	GLuint CS = runner->BuildShaderProgram("shaders/img_ls.comp", GL_COMPUTE_SHADER);

	GLint alive_workgroup_size[3];
	glGetProgramiv(CS, GL_COMPUTE_WORK_GROUP_SIZE, alive_workgroup_size);
	std::cout << ">>>>>>>>> runtime workgroup size(x-y-z) " << alive_workgroup_size[0]
		<< "-" << alive_workgroup_size[1] << "-" << alive_workgroup_size[2] << std::endl;

	GLuint csppline = runner->BuildProgramPipeline();

	glUseProgramStages(csppline, GL_COMPUTE_SHADER_BIT, CS);

	GLuint store_image;
	glGenTextures(1, &store_image);
	glBindTexture(GL_TEXTURE_2D, store_image);
	glTexStorage2D(GL_TEXTURE_2D, 1, GL_RGBA8, GR_WIDTH, GR_HEIGHT);

	glBindImageTexture(0, store_image, 0, GL_FALSE, 0, GL_WRITE_ONLY, GL_RGBA8);

	glDispatchCompute(64, 64, 1);

	// sample and render the previous computed texture
	GLfloat pos_buf[] = {
		-1.0, -1.0,
		-1.0, 1.0,
		1.0, -1.0,
		-1.0, 1.0,
		1.0, -1.0,
		1.0, 1.0
	};

	GLfloat texc_buf[] = {
		0.0, 0.0,
		0.0, 1.0,
		1.0, 0.0,
		0.0, 1.0,
		1.0, 0.0,
		1.0, 1.0
	};

	enum { attr_pos, attr_texc, attr_total };
	GLuint VBO[attr_total];
	glCreateBuffers(attr_total, VBO);
	glNamedBufferStorage(VBO[attr_pos], sizeof(pos_buf), pos_buf, 0);
	glNamedBufferStorage(VBO[attr_texc], sizeof(texc_buf), texc_buf, 0);

	GLuint VAO;
	glCreateVertexArrays(1, &VAO);
	glBindVertexArray(VAO);

	glEnableVertexAttribArray(attr_pos);
	glEnableVertexAttribArray(attr_texc);

	glVertexAttribFormat(attr_pos, 2, GL_FLOAT, GL_FALSE, 0);
	glVertexAttribFormat(attr_texc, 2, GL_FLOAT, GL_FALSE, 0);

	glVertexAttribBinding(attr_pos, 0);
	glVertexAttribBinding(attr_texc, 1);

	glBindVertexBuffer(0, VBO[attr_pos], 0, 2 * sizeof(GLfloat));
	glBindVertexBuffer(1, VBO[attr_texc], 0, 2 * sizeof(GLfloat));

	GLuint VS = runner->BuildShaderProgram("shaders/quad.vert", GL_VERTEX_SHADER);
	GLuint FS = runner->BuildShaderProgram("shaders/quad.frag", GL_FRAGMENT_SHADER);
	GLuint drawppline = runner->BuildProgramPipeline();
	glUseProgramStages(drawppline, GL_VERTEX_SHADER_BIT, VS);
	glUseProgramStages(drawppline, GL_FRAGMENT_SHADER_BIT, FS);

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, store_image);

	glProgramUniform1i(FS, glGetUniformLocation(FS, "color2D"), 0);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

	runner->OnRender();

	return 0;
}
