#version 450 core

layout (points) in;
layout (line_strip, max_vertices = 2) out;

in vec4 geoColor[]; // geomtry input must be array
out vec4 fColor;

void main()
{
	fColor = geoColor[0];
	gl_Position = gl_in[0].gl_Position + vec4(-0.1, 0.0, 0.0, 0.0);
	EmitVertex();

	gl_Position = gl_in[0].gl_Position + vec4(0.1, 0.0, 0.0, 0.0);
	EmitVertex();
}
