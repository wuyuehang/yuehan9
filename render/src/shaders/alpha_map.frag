#version 450 core

precision mediump float;

in vec2 fUV;
out vec4 fColor;

uniform sampler2D tContent2D;
uniform sampler2D tAlpha2D;

void main()
{
	vec3 illumination = vec3(0.2126, 0.7152, 0.0722);

	float brightness = dot(texture(tAlpha2D, fUV).rgb, illumination);

	if (brightness <= 0.2) {
		fColor = texture(tContent2D, fUV);
	} else {
		discard;
	}
}
