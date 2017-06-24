#version 450 core

layout (location = 0) in vec4 vPos;
layout (location = 1) in vec4 vOffset;

out vec4 fVarying0;

void main()
{
	gl_Position = vPos + vec4(vOffset.xyz, 0.0);
	fVarying0 = vOffset;
}
