#version 450 core

layout (location = 0) in vec4 vPos;

out vec2 xfb_pos;

void main()
{
	gl_Position = vPos;
	xfb_pos.x = vPos.x + 1.0;
	xfb_pos.y = vPos.y + 1.0;
}
