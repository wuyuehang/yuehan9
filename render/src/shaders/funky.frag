#version 450 core

out vec4 fColor;

uniform float time_elapsed;
uniform sampler2D color2D;

void main()
{
	fColor = texture(color2D, gl_PointCoord);
	fColor.a = 1.0 - time_elapsed;
}
