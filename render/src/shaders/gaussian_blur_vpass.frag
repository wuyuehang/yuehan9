#version 450 core

precision mediump float;

in vec2 fUV;
out vec4 fColor;

uniform sampler2D tColor2D;
uniform float uGaussianWeights[5];
uniform vec2 uMouse;

void main()
{
	ivec2 resinfo = textureSize(tColor2D, 0);

	float ysnap = 1.0 / float(resinfo.y);

	vec4 n4 = texture(tColor2D, fUV + vec2(0.0, 4.0*ysnap));
	vec4 n3 = texture(tColor2D, fUV + vec2(0.0, 3.0*ysnap));
	vec4 n2 = texture(tColor2D, fUV + vec2(0.0, 2.0*ysnap));
	vec4 n1 = texture(tColor2D, fUV + vec2(0.0, ysnap));
	vec4 m0 = texture(tColor2D, fUV);
	vec4 p1 = texture(tColor2D, fUV + vec2(0.0, -ysnap));
	vec4 p2 = texture(tColor2D, fUV + vec2(0.0, -2.0*ysnap));
	vec4 p3 = texture(tColor2D, fUV + vec2(0.0, -3.0*ysnap));
	vec4 p4 = texture(tColor2D, fUV + vec2(0.0, -4.0*ysnap));

	if (gl_FragCoord.x/resinfo.x < uMouse.x) {
		fColor = \
				uGaussianWeights[0]*n4 + \
				uGaussianWeights[0]*n3 + \
				uGaussianWeights[1]*n2 + \
				uGaussianWeights[2]*n1 + \
				uGaussianWeights[3]*m0 + \
				uGaussianWeights[2]*p1 + \
				uGaussianWeights[1]*p2 + \
				uGaussianWeights[0]*p3 + \
				uGaussianWeights[0]*p4;
	} else {
		fColor = m0;
	}
}
