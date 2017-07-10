#version 450 core

layout (location = 0) in vec4 vPos;
layout (location = 1) in vec3 vNormal;

uniform mat4 uModel;
uniform mat4 uView;
uniform mat4 uProjection;

out vec3 fNormal;
out vec3 fWorld;

void main()
{
	gl_Position = uProjection * uView * uModel * vPos;
	fNormal = mat3(transpose(uModel)) * vNormal;
	fWorld = vec3(uModel * vPos);
}
