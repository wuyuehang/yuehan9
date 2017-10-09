#version 450 core

layout (points) in;
layout (points, max_vertices = 4) out;

out vec4 fColor;

uniform int uNumLayers;

void main()
{
	for (int i = 0; i < uNumLayers; i++) {
		gl_Layer = i;

		gl_Position = gl_in[0].gl_Position;

		fColor = vec4(float(i)/float(uNumLayers));

		EmitVertex();
		EndPrimitive();
	}
}
