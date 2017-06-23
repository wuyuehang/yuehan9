#version 320 es

layout (location = 0) in vec4 vPos;
layout (location = 1) in vec2 vUV;

out vec2 fUV;

void main()
{
	fUV = vUV;
	gl_Position = vPos;
}
