#version 300 es

precision mediump float;
in vec4 frag_color;
out vec4 color;

void main()
{
	color = frag_color;
}