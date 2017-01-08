#version 300 es

layout (location = 0) in vec3 position;
layout (location = 1) in vec2 uv_cord;

uniform mat4 mvp;
out vec2 UV;

void main()
{
	gl_Position = mvp * vec4(position.x, position.y, position.z, 1.0);
	UV = uv_cord;
}
