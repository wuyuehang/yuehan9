#version 330 core

layout (location = 0) in vec2 position;

uniform vec2 offsets[64];

void main()
{
	vec2 offset = offsets[gl_InstanceID];
	gl_Position = vec4(position + offset, 0.0f, 1.0f);
}
