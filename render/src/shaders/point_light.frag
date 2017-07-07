#version 450 core

precision mediump float;

in vec2 fUV;
in vec3 fNormal;
in vec3 fWorld;

out vec4 fColor;

#define diffuse_coeff 1

uniform vec3 uLightLoc;
uniform sampler2D color2D;

void main()
{
	vec3 view_direction = normalize(uLightLoc - fWorld);
	float distance = length(uLightLoc - fWorld);
	float diffuse = max(dot(fNormal, view_direction), 0.0);
	vec4 LightColor = vec4(1.0, 1.0, 0.0, 1.0) * diffuse / (distance * distance + distance);
	fColor = diffuse_coeff * LightColor + texture(color2D, fUV);
}
