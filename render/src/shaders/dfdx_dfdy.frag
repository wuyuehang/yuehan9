#version 450 core

precision highp float;

in vec2 fUV;
out vec4 fColor;

void main()
{
	float xgrad = dFdx(fUV.x);
	float ygrad = dFdy(fUV.y);

	fColor = vec4(xgrad);
}
