#version 450 core

layout (location = 0) in vec4 vPos;

uniform mat4 MVP;

void main()
{
	gl_Position = MVP * vPos;
}
