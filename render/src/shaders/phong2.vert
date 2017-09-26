#version 450 core

layout (location = 0) in vec3 position;
layout (location = 1) in vec3 normal;

out vec3 vs_mv_normal;
out vec3 vs_mv_pos;

uniform mat4 world;
uniform mat4 view;
uniform mat4 proj;

void main()
{
	gl_Position = proj * view * world * vec4(position.x, position.y, position.z, 1.0);

	// calculate light equation in model-view coordinate
	vs_mv_normal = mat3(transpose(inverse(view * world))) * normal;

	vs_mv_pos = vec3(view * world * vec4(position, 1.0f));
}
