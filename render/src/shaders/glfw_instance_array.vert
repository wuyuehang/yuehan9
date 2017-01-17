#version 330 core

layout (location = 0) in vec2 position;
layout (location = 1) in vec2 instance_offset;

void main()
{
	gl_Position = vec4(position + instance_offset, 0.0f, 1.0f);
}
