#version 450 core

#define darkarea_coeff 0.6

precision mediump float;

in vec2 fUV;
in vec3 fNormal;
in vec3 fWorld;

out vec4 fColor;

uniform vec3 uLightLoc;
uniform vec3 uAntenna;
uniform float uRadians;
uniform sampler2D color2D;

void main()
{
	vec3 view_dir = normalize(uLightLoc - fWorld);
	float range = dot(-view_dir, normalize(uAntenna));

	if (range < cos(uRadians)) {
		// dark area
		fColor = darkarea_coeff * texture(color2D, fUV);
	} else {
		float distance = length(uLightLoc - fWorld);
		float diffuse = max(dot(normalize(fNormal), view_dir), 0.0);
		vec4 LightColor = vec4(0.5, 0.5, 0.0, 1.0) * diffuse / (distance * distance + distance);
		fColor = LightColor + texture(color2D, fUV);
	}
}
