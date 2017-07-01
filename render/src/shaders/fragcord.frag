#version 450

#define _enable_z_ 0

uniform vec2 resinfo;

out vec4 fColor;

void main()
{
	fColor.r = gl_FragCoord.x / resinfo.r;
	fColor.g = gl_FragCoord.y / resinfo.g;
#if _enable_z_
	fColor.b = gl_FragCoord.z;
	fColor.a = 0.0;
#else
	fColor.ba = vec2(0.0);
#endif
}
