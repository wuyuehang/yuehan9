#version 450 core

#define coeffient 0.1

precision highp float;

in vec2 fUV;
out vec4 fColor;

uniform sampler2D mipmapTexture2D;
uniform mat4 uWorld;

void main()
{
	float xgrad = dFdx(fUV.x);
	float ygrad = dFdy(fUV.y);

	/* lod.x return array index */
	/* lod.y return gpu computed lod */
	vec2 gpu_lod = textureQueryLod(mipmapTexture2D, fUV);

	fColor = texture(mipmapTexture2D, fUV);

	fColor = vec4(gpu_lod.y * coeffient);
}
