#version 330 core

layout (location = 0) in vec3 position;
layout (location = 1) in vec2 texcord;

out vec2 frag_texcord;

void main()
{
	gl_Position = vec4(position.x, position.y, position.z, 1.0);
	frag_texcord = texcord;
}
