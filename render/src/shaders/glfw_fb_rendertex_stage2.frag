#version 330 core

in vec2 frag_texcord;
out vec4 color;

uniform sampler2D v4v_tex2d;

void main()
{
	color = vec4(texture(v4v_tex2d, frag_texcord).rgb, 1.0);
}
