#version 450 core

layout (location = 0) in vec4 vPos;
layout (location = 1) in vec2 vUV;

out vec2 fUV;

uniform mat4 uWorld;

void main()
{
	fUV = vUV;
	gl_Position = uWorld * vPos;
}
