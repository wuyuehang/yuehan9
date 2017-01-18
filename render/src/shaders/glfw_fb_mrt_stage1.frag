#version 330 core

in vec3 vtx_world_pos;

layout (location = 0) out vec4 color;
layout (location = 1) out vec4 frg_pos;

void main()
{
	color = vec4(0.0f, 1.0f, 1.0f, 1.0f);
	frg_pos = vec4(vtx_world_pos, 1.0f);
}
