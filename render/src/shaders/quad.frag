#version 450 core

precision mediump float;

in vec2 fUV;
out vec4 fColor;

uniform sampler2D color2D;

void main()
{
	fColor = texture(color2D, fUV);
}
