#version 450 core

in vec4 fVarying0;

layout (location = 0) out vec4 mrt0;
layout (location = 1) out vec4 mrt1;

void main()
{
	mrt0 = vec4(0.0f, 1.0f, 1.0f, 1.0f);
	mrt1 = fVarying0;
}
