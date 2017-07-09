#version 450 core

in vec4 varyingColor;
out vec4 fColor;

void main()
{
	fColor = varyingColor;
}
