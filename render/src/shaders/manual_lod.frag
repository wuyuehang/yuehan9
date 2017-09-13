#version 450 core

precision highp float;

in vec2 fUV;
out vec4 fColor;

uniform sampler2D mipmapTexture2D;
uniform mat4 uWorld;

void main()
{
	ivec2 resinfo = textureSize(mipmapTexture2D, 0);
	float xgrad = dFdx(fUV.x*resinfo.x);
	float ygrad = dFdy(fUV.y*resinfo.y);

	float lamda = max(xgrad*xgrad, ygrad*ygrad);

	lamda = 0.5*log2(lamda);

	fColor = vec4(lamda);
}
