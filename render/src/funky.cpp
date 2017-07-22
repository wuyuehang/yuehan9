#include "glrunner.h"
#include <SOIL/SOIL.h>
#include <ctime>
#include <cstdlib>

#define PN	10000
GLuint vsProgram;
GLuint fsProgram;

void RenderCB(GlRunner *runner)
{
	static float delta = 0.0;
	clock_t t1 = clock();

	glClearColor(0.15, 0.05, 0.15, 0.0);
	glClearDepth(1.0);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	clock_t t2 = clock();
	delta += (t2 - t1);

	if (delta > 100000.0)
		delta = 0.0;

	glProgramUniform1f(vsProgram, glGetUniformLocation(vsProgram, "time_elapsed"), delta / 100000.0);
	glProgramUniform1f(fsProgram, glGetUniformLocation(fsProgram, "time_elapsed"), delta / 100000.0);

	glDrawArrays(GL_POINTS, 0, PN);
}

int main()
{
	GlRunner *runner = new GlRunner(RenderCB);

	glEnable(GL_PROGRAM_POINT_SIZE);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ZERO);

	vsProgram = runner->BuildShaderProgram("shaders/funky.vert", GL_VERTEX_SHADER);

	fsProgram = runner->BuildShaderProgram("shaders/funky.frag", GL_FRAGMENT_SHADER);

	GLuint ppline = runner->BuildProgramPipeline();

	glUseProgramStages(ppline, GL_VERTEX_SHADER_BIT, vsProgram);
	glUseProgramStages(ppline, GL_FRAGMENT_SHADER_BIT, fsProgram);

	// cpu prepare resouce
	std::srand(std::time(0));
	GLfloat pos[PN*3];	// initial position of center
	GLfloat wind[PN];	// winding direction
	GLfloat atom[PN]; // initial point size

	GLfloat *pPos = &pos[0];
	GLfloat *pWind = &wind[0];
	GLfloat *pAtom = &atom[0];

	for (int i = 0; i < PN; i++) {
		*pPos = 2 * float(std::rand() % 100) / 100.0 - 1.0;
		*(pPos+1) = 2 * float(std::rand() % 100) / 100.0 - 1.0;
		*(pPos+2) = 2 * float(std::rand() % 100) / 100.0 - 1.0;
		pPos += 3;

		*pWind = (std::rand() % 2 == 1) ? 1.0 : -1.0;
		pWind += 1;

		*pAtom = float((std::rand() % 25));
		pAtom += 1;
	}

	// gpu prepare resource
	GLuint VAO;
	glGenVertexArrays(1, &VAO);
	glBindVertexArray(VAO);
	GLuint VBO[3];
	glGenBuffers(3, VBO);

	glBindBuffer(GL_ARRAY_BUFFER, VBO[0]);
	glBufferData(GL_ARRAY_BUFFER, sizeof(pos), pos, GL_STATIC_DRAW);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3*sizeof(GLfloat), (GLvoid *)0);

	glBindBuffer(GL_ARRAY_BUFFER, VBO[1]);
	glBufferData(GL_ARRAY_BUFFER, sizeof(wind), wind, GL_STATIC_DRAW);
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 1, GL_FLOAT, GL_FALSE, sizeof(GLfloat), (GLvoid *)0);

	glBindBuffer(GL_ARRAY_BUFFER, VBO[2]);
	glBufferData(GL_ARRAY_BUFFER, sizeof(atom), atom, GL_STATIC_DRAW);
	glEnableVertexAttribArray(2);
	glVertexAttribPointer(2, 1, GL_FLOAT, GL_FALSE, sizeof(GLfloat), (GLvoid *)0);

	// loading texture image
	GLuint textures[1];
	glGenTextures(1, textures);

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, textures[0]);
	int width, height, channel;
	unsigned char *img = SOIL_load_image("materials/circle_particles.jpg", &width, &height, &channel, SOIL_LOAD_RGBA);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, img);
	SOIL_free_image_data(img);

	glProgramUniform1i(fsProgram, glGetUniformLocation(fsProgram, "color2D"), 0);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	runner->OnRender();

	return 0;
}
