#version 320 es

layout (location = 0) in vec4 vPos;
layout (location = 1) in vec4 vColor;

out vec4 fColor;

void main()
{
	fColor = vColor;
	gl_Position = vPos;
}
