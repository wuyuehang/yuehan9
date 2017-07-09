#version 450 core

layout (location = 0) in vec4 vPos;
layout (location = 1) in vec4 vColor;

uniform mat4 uModel;
uniform mat4 uView;
uniform mat4 uProjection;

out vec4 varyingColor;

void main()
{
	varyingColor = vColor;
	gl_Position = uProjection * uView * uModel * vPos;
}
