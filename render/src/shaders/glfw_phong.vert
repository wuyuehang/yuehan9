#version 330 core

layout (location = 0) in vec3 position;
layout (location = 1) in vec3 normal;

out vec3 vs_normal;
out vec3 vs_world_pos;

uniform mat4 world;
uniform mat4 view;
uniform mat4 proj;

void main()
{
	gl_Position = proj * view * world * vec4(position.x, position.y, position.z, 1.0);

	vs_normal = mat3(transpose(inverse(world))) * normal;

	vs_world_pos = vec3(world * vec4(position, 1.0f));
}
