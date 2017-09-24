#version 450 core

precision mediump float;

in vec2 fUV;
out vec4 fColor;

uniform sampler2D tColor2D;
uniform float uGaussianWeights[5];

void main()
{
	ivec2 resinfo = textureSize(tColor2D, 0);

	float xsnap = 1.0 / float(resinfo.x);

	vec4 n4 = texture(tColor2D, fUV + vec2(-4.0*xsnap, 0.0));
	vec4 n3 = texture(tColor2D, fUV + vec2(-3.0*xsnap, 0.0));
	vec4 n2 = texture(tColor2D, fUV + vec2(-2.0*xsnap, 0.0));
	vec4 n1 = texture(tColor2D, fUV + vec2(-xsnap, 0.0));
	vec4 m0 = texture(tColor2D, fUV);
	vec4 p1 = texture(tColor2D, fUV + vec2(xsnap, 0.0));
	vec4 p2 = texture(tColor2D, fUV + vec2(2.0*xsnap, 0.0));
	vec4 p3 = texture(tColor2D, fUV + vec2(3.0*xsnap, 0.0));
	vec4 p4 = texture(tColor2D, fUV + vec2(4.0*xsnap, 0.0));

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
}
