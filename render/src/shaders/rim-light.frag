#version 450 core

in vec3 vs_normal;
in vec3 vs_world_pos;

out vec4 color;

uniform vec3 light_pos;
uniform vec3 view_pos;

void main()
{
	vec3 view_dir = view_pos - vs_world_pos;

	float rim = 1.0 - dot(normalize(view_dir), normalize(vs_normal));

	rim = smoothstep(0.0, 1.0, rim);

	color = vec4(1.0)*pow(rim, 8);
}
