#version 450 core

layout (location = 0) in vec4 vPos;
layout (location = 1) in vec2 vUV;

uniform mat4 uMVP;
noperspective out vec2 fUV;

void main()
{
	fUV = vUV;
	gl_Position = uMVP * vPos;
}
