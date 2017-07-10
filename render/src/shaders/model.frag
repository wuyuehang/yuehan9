#version 450 core

#define LIGHTCOLOR vec4(1.0, 1.0, 1.0, 1.0)

in vec3 fNormal;
in vec3 fWorld;

uniform vec3 uLightLoc;

out vec4 fColor;

void main()
{
	vec3 light_dir = normalize(uLightLoc - fWorld);

	float distance = length(uLightLoc - fWorld);

	float diffuse = max(dot(normalize(fNormal), light_dir), 0.0);

	fColor = vec4(0.25, 0.25, 0.25, 1.0) + diffuse * LIGHTCOLOR;
}
