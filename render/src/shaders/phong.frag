#version 330 core

in vec3 vs_normal;
in vec3 vs_world_pos;

out vec4 color;

uniform vec3 light_pos;
uniform vec3 view_pos;

void main()
{
	// diffuse
	vec3 light_dir = normalize(light_pos - vs_world_pos);
	float diffuse = max(dot(vs_normal, light_dir), 0);
	color = (0.05f + diffuse) * vec4(0.8f, 0.8f, 0.0f, 1.0f);

	// specular
	vec3 view_dir = normalize(view_pos - vs_world_pos);
	vec3 reflect_dir = reflect(-light_dir, vs_normal);
	float specular = pow(max(dot(view_dir, reflect_dir), 0.0f), 16);
	color = color + 0.95f * specular * vec4(0.8f, 0.8f, 0.0f, 1.0f);

	// self color
	color = color + vec4(0.2f, 0.0f, 0.0f, 0.0f);
	color = clamp(color, vec4(0.0f, 0.0f, 0.0f, 0.0f), vec4(1.0f, 1.0f, 1.0f, 1.0f));
}
