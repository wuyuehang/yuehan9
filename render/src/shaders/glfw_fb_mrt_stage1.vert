#version 330 core

layout (location = 0) in vec2 position;
layout (location = 1) in vec2 instance_offset;

uniform vec2 offsets[64];

out vec3 vtx_world_pos;

void main()
{
	gl_Position = vec4(position + instance_offset, 0.0f, 1.0f);
	vtx_world_pos = vec3(position + instance_offset, 0.0f);
}
