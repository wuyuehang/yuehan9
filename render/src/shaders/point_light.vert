#version 450 core

layout (location = 0) in vec4 vPos;
layout (location = 1) in vec2 vUV;
layout (location = 2) in vec3 vNormal;

uniform mat4 uModel;
uniform mat4 uView;
uniform mat4 uProj;

out vec2 fUV;
out vec3 fNormal;
out vec3 fWorld;

void main()
{
	fUV = vUV;
	gl_Position = uProj * uView * uModel * vPos;
	fNormal = mat3(transpose(uModel)) * vNormal;
	fWorld = vec3(uModel * vPos);
}
