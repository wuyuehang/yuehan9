#version 450 core

precision highp float;

in vec2 fUV;
out vec4 fColor;

uniform sampler2D tColor2D;

#define _gradient_threshold 0.5
#define _hist_num 8.0

void main()
{
	ivec2 resinfo = textureSize(tColor2D, 0);

	float xsnap = 1.0 / float(resinfo.x);
	float ysnap = 1.0 / float(resinfo.y);
	vec3 illumination = vec3(0.2126, 0.7152, 0.0722);

	// 3x3 candidate texel
	float p00 = dot(texture(tColor2D, fUV + vec2(-xsnap, ysnap)).rgb, illumination);
	float p10 = dot(texture(tColor2D, fUV + vec2(-xsnap, 0.0)).rgb, illumination);
	float p20 = dot(texture(tColor2D, fUV + vec2(-xsnap, -ysnap)).rgb, illumination);

	float p01 = dot(texture(tColor2D, fUV + vec2(0.0, ysnap)).rgb, illumination);
	//float p11 = dot(texture(tColor2D, fUV + vec2(0.0, 0.0)).rgb, illumination); // not used
	float p21 = dot(texture(tColor2D, fUV + vec2(0.0, -ysnap)).rgb, illumination);

	float p02 = dot(texture(tColor2D, fUV + vec2(xsnap, ysnap)).rgb, illumination);
	float p12 = dot(texture(tColor2D, fUV + vec2(xsnap, 0.0)).rgb, illumination);
	float p22 = dot(texture(tColor2D, fUV + vec2(xsnap, -ysnap)).rgb, illumination);

	// perform x direction detection
	float xdelta = p00 + 2.0*p01 + p02 - p20 - 2.0*p21 - p22;

	// perform y direction detection
	float ydelta = p00 + 2.0*p10 + p20 - p02 - 2.0*p12 - p22;

	float calc_gradient = xdelta*xdelta + ydelta*ydelta;

	if (calc_gradient >= _gradient_threshold) {
		fColor = vec4(0.0);
	} else {
		// histogram the continuous color
		fColor = floor(_hist_num * texture(tColor2D, fUV)) / _hist_num;
	}
}
