#version 450 core

layout (location = 0) in vec4 vPos;
layout (location = 1) in vec4 vColor;

uniform MVP {
	mat4 uModel;
	mat4 uView;
	mat4 uProj;
};

out vec4 fColor;

void main()
{
	mat4 vMVP = uProj * uView * uModel;
	fColor = vColor;
	gl_Position = vMVP * vPos;
}

