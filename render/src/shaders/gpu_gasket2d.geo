#version 450 core

#define _num_sierpinki 256
#define amp	1.5

layout (triangles) in;
layout (points, max_vertices = _num_sierpinki) out;

uniform float uTime;

void main()
{
	vec4 next_point = vec4(0.0, 0.0, 0.0, 1.0);
	int next_index;

	for (int i = 0; i < _num_sierpinki; i++) {

		// a math pattern to simulate index generation
		next_index = int(mod(pow(i, amp+amp*sin(uTime)), 3));

		vec4 pick_endpoint = gl_in[next_index].gl_Position;

		next_point.xy = 0.5 * (next_point.xy + pick_endpoint.xy);

		gl_Position = next_point;

		EmitVertex();

		EndPrimitive();
	}
}
