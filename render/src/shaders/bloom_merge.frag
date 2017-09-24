#version 450 core

precision mediump float;

in vec2 fUV;
out vec4 fColor;

uniform float uTime;
uniform sampler2D tBlur2D;
uniform sampler2D tOrigin2D;

#define downA	0.0
#define upA 8.0

void main()
{
	fColor = (0.5*(upA-downA)*sin(1.5*uTime)+0.5*(upA+downA))*texture(tBlur2D, fUV) + texture(tOrigin2D, fUV);
}
