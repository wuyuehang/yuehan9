#version 300 es

precision mediump float;

in vec2 UV;
out vec4 color;

uniform sampler2D v4v_tex2d;

void main()
{
	color = vec4(texture(v4v_tex2d, UV).rgb, 1.0);
}
