#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <SOIL/SOIL.h>
#include "warp_types.h"

int main()
{
	struct offscreen_warp *ow;
	ow = (struct offscreen_warp *)malloc(sizeof(*ow));
	memset(ow, 0, sizeof(*ow));
	unsigned char *blob = (unsigned char *)malloc(WARP_SURFACE_WIDTH * WARP_SURFACE_HEIGHT * 4);

	create_offscreen_warp(ow);

	// preamble
	create_offscreen_warp_shaders(ow,
			&ow->vertex_shader[0], "shaders/es3_draw_tri.vert",
			&ow->fragment_shader[0], "shaders/es3_draw_tri.frag");

	create_offscreen_warp_program(&ow->vertex_shader[0],
			&ow->fragment_shader[0],
			&ow->program[0]);

	// rendering happen
	GLfloat pos_buf[] = {
		-0.5, -0.5,
		0.0, 0.5,
		0.5, -0.5
	};

	GLushort idx_buf[] = {
		0, 1, 2
	};

	GLuint vao, vbo_idx, vbo_pos;
	glGenVertexArrays(1, &vao);
	glGenBuffers(1, &vbo_idx);
	glGenBuffers(1, &vbo_pos);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, vbo_idx);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(idx_buf), idx_buf, GL_STATIC_DRAW);
	glBindBuffer(GL_ARRAY_BUFFER, vbo_pos);
	glBufferData(GL_ARRAY_BUFFER, sizeof(pos_buf), pos_buf, GL_STATIC_DRAW);

	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(GLfloat), (GLvoid *)0);
	glEnableVertexAttribArray(0);

	glDrawElements(GL_TRIANGLES, 3, GL_UNSIGNED_SHORT, (void *)0);

	// dump the image
	glReadPixels(0, 0, WARP_SURFACE_WIDTH, WARP_SURFACE_HEIGHT, GL_RGBA, GL_UNSIGNED_BYTE, blob);
	SOIL_save_image(WARP_IMAGE,
			SOIL_SAVE_TYPE_BMP,
			WARP_SURFACE_WIDTH,
			WARP_SURFACE_HEIGHT,
			4, blob);
	destroy_offscreen_warp(ow);

	return 0;
}
