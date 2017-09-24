#version 450 core

precision highp float;

in vec2 fUV;

out vec4 fColor;

uniform sampler2D tBrightness2D;

void main()
{
	vec4 val = texture(tBrightness2D, fUV);

	float illumination = dot(vec3(0.2126, 0.7152, 0.0722), val.rgb);

	if (illumination > 0.4) {
		fColor = vec4(1.0);
	} else {
		fColor = vec4(0.0);
	}
}
