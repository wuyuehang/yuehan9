#version 450 core

layout (location = 0) in vec4 vPos;
layout (location = 1) in vec4 vColor;

layout (binding = 0, offset = 0) uniform atomic_uint atcVertice;

out vec4 geoColor;

void main()
{
	geoColor = vColor;
	gl_Position = vPos;
	// increase
	atomicCounterIncrement(atcVertice);
}
