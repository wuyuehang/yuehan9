#version 450 core

#define PI 3.1415926
#define RADIUS (1.0 - time_elapsed)
#define PSIZE 50

layout (location = 0) in vec4 vPos;
layout (location = 1) in float vWind;
layout (location = 2) in float vAtom;

uniform float time_elapsed;

void main()
{
	gl_Position.zw = vPos.zw;
	gl_Position.x = vPos.x + RADIUS * cos(vWind * pow(smoothstep(0.0, 1.0, time_elapsed), 3) * 2 * PI);
	gl_Position.y = vPos.y + RADIUS * sin(vWind * pow(smoothstep(0.0, 1.0, time_elapsed), 3) * 2 * PI);
	gl_PointSize = (1.0 - time_elapsed) * vAtom;
}
