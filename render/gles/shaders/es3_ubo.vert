#version 300 es

precision mediump float;

layout (location = 0) in vec3 vposition;
layout (location = 1) in vec4 vcolor;

uniform MVP {
	mat4 uModel;
	mat4 uView;
	mat4 uProj;
};

out vec4 fcolor;

void main()
{
	mat4 vMVP = uProj * uView * uModel;
	gl_Position = vMVP * vec4(vposition.x, vposition.y, vposition.z, 1.0);
	fcolor = vcolor;
}
