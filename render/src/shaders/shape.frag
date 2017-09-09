#version 450

/* used by pixel_karate demo */
precision highp float;

uniform uvec2 uResInfo;
uniform float uTime;
uniform vec2 uMouse;

out vec4 fColor;

#define T0	0.05
#define A	(0.5+0.5*sin(uTime))
#define A0	(0.25+0.25*sin(uTime))
#define MAX_QUAD	100

/* plot square at center texture coordinate inside [0, 1] */
float PlotSquareAt(vec2 center, vec2 loc)
{
	float res;

	res = step(A*center.x, loc.x);
	res *= step(A*center.y, loc.y);
	res *= step(A*(1.0-center.x), 1.0-loc.x);
	res *= step(A*(1.0-center.y), 1.0-loc.y);

	return res;
}

/* smooth plot square at center texture coordinate inside [0, 1] */
float SmoothPlotSquareAt(vec2 center, vec2 loc)
{
	float res;

	float t0 = A*center.x;
	float t1 = A*center.y;
	float t2 = A*(1.0-center.x);
	float t3 = A*(1.0-center.y);

	res = smoothstep(t0, t0+T0, loc.x);
	res *= smoothstep(t1, t1+T0, loc.y);
	res *= smoothstep(t2, t2+T0, 1.0-loc.x);
	res *= smoothstep(t3, t3+T0, 1.0-loc.y);

	return res;
}

/* varying frequency and smooth plot square at center texture coordinate inside [0, 1] */
float VaryingSmoothPlotSquareAt(vec2 center, vec2 loc, float freq)
{
#define VSPSA_T0	0.0025
	float res;
	float varying_wave = (0.5+0.5*sin(freq));

	float t0 = varying_wave*center.x;
	float t1 = varying_wave*center.y;
	float t2 = varying_wave*(1.0-center.x);
	float t3 = varying_wave*(1.0-center.y);

	res = smoothstep(t0, t0+VSPSA_T0, loc.x);
	res *= smoothstep(t1, t1+VSPSA_T0, loc.y);
	res *= smoothstep(t2, t2+VSPSA_T0, 1.0-loc.x);
	res *= smoothstep(t3, t3+VSPSA_T0, 1.0-loc.y);

	return res;
}

void main()
{
	float amplifier[2];
	float w[2];
	float x = gl_FragCoord.x / uResInfo.x;
	float y = gl_FragCoord.y / uResInfo.y;
	float fc = 0.0;
	fColor = vec4(0.0);

#if 0
	amplifier[0] = A0;
	fc = step(A0, x);
	fc *= step(A0, y);
	fc *= step(A0, 1.0-x);
	fc *= step(A0, 1.0-y);
	fColor += amplifier[0]*vec4(vec2(fc), vec2(0.0));
#endif

#if 0
	amplifier[1] = 2.0*A0;
	fc = smoothstep(A0, A0+T0, x);
	fc *= smoothstep(A0, A0+T0, y);
	fc *= smoothstep(A0, A0+T0, 1.0-x);
	fc *= smoothstep(A0, A0+T0, 1.0-y);
	fColor += amplifier[1]*vec4(fc, vec3(0.0));
#endif

#if 0
	fc = SmoothPlotSquareAt(vec2(0.5), vec2(x, y));
	fColor += A0*vec4(fc, vec3(0.0));

	fc = SmoothPlotSquareAt(vec2(0.25), vec2(x, y));
	fColor += A0*vec4(0.0, fc, vec2(0.0));

	fc = SmoothPlotSquareAt(vec2(0.75), vec2(x, y));
	fColor += A0*vec4(vec2(0.0), fc, 0.0);

	fc = SmoothPlotSquareAt(vec2(0.25, 0.75), vec2(x, y));
	fColor += A0*vec4(vec2(fc), vec2(0.0));

	fc = SmoothPlotSquareAt(vec2(0.75, 0.25), vec2(x, y));
	fColor += A0*vec4(fc, 0.0, fc, 0.0);
#endif

#if 0
	int i;
	float vary_illumi;
	float vary_frequency;
	float vary_wave;
	float sum_illumi = 0.0;

	for (i = 1; i <= MAX_QUAD; i++) {

		vary_illumi = smoothstep(0.0, 1.0, float(i)/MAX_QUAD);
		vary_frequency = float(i)/MAX_QUAD*uTime;
		vary_wave = 0.25+0.25*sin(vary_frequency);

		fc = smoothstep(vary_wave, vary_wave+0.05, x);
		fc *= smoothstep(vary_wave, vary_wave+0.05, y);
		fc *= smoothstep(vary_wave, vary_wave+0.05, 1.0-x);
		fc *= smoothstep(vary_wave, vary_wave+0.05, 1.0-y);
		fColor += vary_illumi*vec4(0.0, fc, vec2(0.0));

		sum_illumi += vary_illumi;
	}

	fColor /= sum_illumi;
#endif

#if 1
	int i;
	float vary_illumi;
	float vary_frequency;
	float dvary_frequency;
	float sum_illumi = 0.0;

	for (i = 1; i <= MAX_QUAD; i++) {

		vary_illumi = smoothstep(0.0, 1.0, float(i)/MAX_QUAD);

		dvary_frequency = 0.75+0.75*sin(uTime);
		vary_frequency = dvary_frequency*float(i)/MAX_QUAD*uTime;

		// middle
		fc = VaryingSmoothPlotSquareAt(vec2(0.5), vec2(x, y), vary_frequency);
		fColor += vary_illumi*vec4(0.0, fc, 0.0, 0.0);

		// bottom-left
		fc = VaryingSmoothPlotSquareAt(vec2(0.0), vec2(x, y), vary_frequency);
		fColor += vary_illumi*vec4(fc, 0.0, 0.0, 0.0);

		// top-right
		fc = VaryingSmoothPlotSquareAt(vec2(1.0), vec2(x, y), vary_frequency);
		fColor += vary_illumi*vec4(0.0, 0.0, fc, 0.0);

		// top-left
		fc = VaryingSmoothPlotSquareAt(vec2(0.0, 1.0), vec2(x, y), vary_frequency);
		fColor += vary_illumi*vec4(fc, 0.0, fc, 0.0);

		// bottom-right
		fc = VaryingSmoothPlotSquareAt(vec2(1.0, 0.0), vec2(x, y), vary_frequency);
		fColor += vary_illumi*vec4(fc, fc, 0.0, 0.0);

		sum_illumi += vary_illumi;
	}

	fColor = fColor / sum_illumi;
#endif
}
