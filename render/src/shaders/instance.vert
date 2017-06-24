#version 450 core

layout (location = 0) in vec4 vPos;
layout (location = 1) in vec4 vOffset;

void main()
{
	gl_Position = vPos + vOffset;
}
