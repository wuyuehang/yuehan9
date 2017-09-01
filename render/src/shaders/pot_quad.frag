#version 450 core

precision mediump float;

out vec4 fColor;

uniform sampler2D color2D;

void main()
{
	ivec2 pick = ivec2(gl_FragCoord.xy - vec2(0.5, 0.5));
	int i = pick.x;
	int j = pick.y;
	fColor = 0.25 * (texelFetch(color2D, ivec2(2*i, 2*j), 0) +
		texelFetch(color2D, ivec2(2*i+1, 2*j), 0) +
		texelFetch(color2D, ivec2(2*i, 2*j+1), 0) +
		texelFetch(color2D, ivec2(2*i+1, 2*j+1), 0));
}
