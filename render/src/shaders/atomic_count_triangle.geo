#version 450 core

layout (points) in;
layout (triangle_strip, max_vertices = 4) out;

layout (binding = 1, offset = 0) uniform atomic_uint atcPrimitive;

in vec4 geoColor[]; // geometry input must be array;
out vec4 fColor;

void main()
{
	fColor = geoColor[0];
	gl_Position = gl_in[0].gl_Position + vec4(-0.1, -0.1, 0.0, 0.0);
	EmitVertex();

	gl_Position = gl_in[0].gl_Position + vec4(0.1, -0.1, 0.0, 0.0);
	EmitVertex();

	gl_Position = gl_in[0].gl_Position + vec4(0.1, 0.1, 0, 0);
	EmitVertex();

	gl_Position = gl_in[0].gl_Position + vec4(-0.1, 0.1, 0, 0);
	EmitVertex();

	EndPrimitive();

	atomicCounterIncrement(atcPrimitive);
	atomicCounterIncrement(atcPrimitive);
}
