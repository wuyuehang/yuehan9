#version 320 es

precision mediump float;

in vec2 fUV;
out vec4 fColor;

uniform sampler2D color2D;

void main()
{
	//fColor = vec4(texture(depth2D, fUV).r, 1.0, 1.0, 1.0);
	fColor = texture(color2D, fUV);
	//fColor = vec4(1.0, 1.0, 1.0, 1.0);
}
