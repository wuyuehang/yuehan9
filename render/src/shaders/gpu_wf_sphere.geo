#version 450 core

#define _num_detailed 256
#define _incremental 12
#define _pi 3.1415926

layout (points) in;
layout (line_strip, max_vertices = _num_detailed) out;

uniform mat4 uMVP;

void main()
{
	vec4 center = gl_in[0].gl_Position;

	/* generate longtitude vertex */
	for (int i = 0; i <= 360; i+= _incremental) {
		for (int j = 0; j <= 180; j+= _incremental) {
			gl_Position.x = cos(i*_pi/180.0) * sin(j*_pi/180.0);
			gl_Position.z = sin(i*_pi/180.0) * sin(j*_pi/180.0);
			gl_Position.y = cos(j*_pi/180.0);

			gl_Position += gl_in[0].gl_Position;

			gl_Position.w = 1.0;
			gl_Position *= uMVP;

			EmitVertex();
		}
		EndPrimitive();
	}
}
