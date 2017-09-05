#version 450

precision highp float;

uniform uvec2 uResInfo;
uniform float uTime;
uniform vec2 uMouse;

out vec4 fColor;

float QuadraticBezier(vec2 a, float x)
{
	float delta = sqrt(a.x*a.x+x-2.0*a.x*x);

	float root = (-a.x+delta)/(1.0-2.0*a.x);

	return 2.0*a.y*root+(1.0-2.0*a.y)*root*root;
}

float CubicBezier(vec2 a0, vec2 a1, float x)
{
	// cubic bezier interpolation
	// x = (1+3a0-3a1)*t^3 + (3a1-6a0)*t^2 + 3a0*t;
	// y = (1+3a0-3a1)*t^3 + (3a1-6a0)*t^2 + 3a0*t;

	// solve t
	// f(t) = (1+3a0-3a1)*t^3 + (3a1-6a0)*t^2 + 3a0*t - x;
	// f'(t) = (3+9a0-9a1)*t^2 + (6a1-12a0)*t + 3a0;

	int i;

	float st = x;
	float st_next;
	float ft;
	float dft;

	for(i = 0; i < 200; i++) {

		// st_next = st - f(st) / f'(st);

		ft = (1.0+3.0*a0.x-3.0*a1.x)*st*st*st + (3.0*a1.x-6.0*a0.x)*st*st + 3.0*a0.x*st - x;
		dft = (3.0+9.0*a0.x-9.0*a1.x)*st*st + (6.0*a1.x-12.0*a0.x)*st + 3.0*a0.x;

		if (abs(dft) <= 0.00001) {
			st_next = st;
		} else {
			st_next = st - ft / dft;
		}

		st = st_next;
	}

	return (1.0+3.0*a0.y-3.0*a1.y)*st*st*st + (3.0*a1.y-6.0*a0.y)*st*st + 3.0*a0.y*st;
}

float FourthOrderBezier(vec2 a0, vec2 a1, vec2 a2, float x)
{
	// cubic bezier interpolation
	// x = (1-4a0+6a1-4a2)*t^4 + (12a0-12a1+4a2)*t^3 + (-12a0+6a1)*t^2 + 4a0*t;
	// y = (1-4a0+6a1-4a2)*t^4 + (12a0-12a1+4a2)*t^3 + (-12a0+6a1)*t^2 + 4a0*t;

	// solve t
	// f(t) = (1-4a0+6a1-4a2)*t^4 + (12a0-12a1+4a2)*t^3 + (-12a0+6a1)*t^2 + 4a0*t - x;
	// f'(t) = (4-16a0+24a1-16a2)*t^3 + (36a0-36a1+12a2)*t^6 + (-24a0+12a1)*t + 4a0;

	int i;

	float st = x;
	float st_next;
	float ft;
	float dft;

	for(i = 0; i < 200; i++) {

		// st_next = st - f(st) / f'(st);

		ft = (1.0-4.0*a0.x+6.0*a1.x-4.0*a2.x)*st*st*st*st + (12.0*a0.x-12.0*a1.x+4.0*a2.x)*st*st*st + (-12.0*a0.x+6.0*a1.x)*st*st + 4.0*a0.x*st - x;
		dft = (4.0-16.0*a0.x+24.0*a1.x-16.0*a2.x)*st*st*st + (36.0*a0.x-36.0*a1.x+12.0*a2.x)*st*st + (-24.0*a0.x+12.0*a1.x)*st + 4.0*a0.x;

		if (abs(dft) <= 0.00001) {
			st_next = st;
		} else {
			st_next = st - ft / dft;
		}

		st = st_next;
	}

	return (1.0-4.0*a0.y+6.0*a1.y-4.0*a2.y)*st*st*st*st + (12.0*a0.y-12.0*a1.y+4.0*a2.y)*st*st*st + (-12.0*a0.y+6.0*a1.y)*st*st + 4.0*a0.y*st;
}

void main()
{
	float w[2];
	float x = gl_FragCoord.x / uResInfo.x;
	float y = gl_FragCoord.y / uResInfo.y;
	float fc;
	float radius;
	fColor = vec4(0.0);

	// draw animation point
	vec2 ap = 0.5*vec2(cos(uTime), sin(uTime))+0.5;
	radius = distance(ap, vec2(x, y));
	w[0] = smoothstep(0.0045, 0.006, radius);
	fColor += vec4(1.0 - w[0].r, vec3(0.0));

	// quadratic
	fc = QuadraticBezier(ap, x);

	w[0] = smoothstep(fc-0.01, fc, y);
	w[1] = smoothstep(fc, fc+0.01, y);
	fColor += vec4(w[0]-w[1], vec3(0.0));

	// cubic bezier
	// 1st dynamic control point
	vec2 cubic_ap0 = vec2(0.25, 0.25*sin(2.0*uTime)+0.5);
	radius = distance(cubic_ap0, vec2(x, y));
	w[0] = smoothstep(0.0045, 0.006, radius);
	fColor += vec4(0.0, 1.0 - w[0].r, vec2(0.0));

	// 2nd dynamic control point
	vec2 cubic_ap1 = vec2(0.75, 0.25*cos(2.0*uTime)+0.5);
	radius = distance(cubic_ap1, vec2(x, y));
	w[0] = smoothstep(0.0045, 0.006, radius);
	fColor += vec4(0.0, 1.0 - w[0].r, vec2(0.0));

	// cubic
	fc = CubicBezier(cubic_ap0, cubic_ap1, x);

	w[0] = smoothstep(fc-0.01, fc, y);
	w[1] = smoothstep(fc, fc+0.01, y);
	fColor += vec4(0.0, w[0]-w[1], vec2(0.0));

	// fourth order bezier
	// 1st dynamic control point
	vec2 fourth_ap0 = vec2(0.25, 0.25*sin(4.0*uTime)+0.5);
	radius = distance(fourth_ap0, vec2(x, y));
	w[0] = smoothstep(0.0045, 0.006, radius);
	fColor += vec4(vec2(0.0), 1.0 - w[0].r, 0.0);

	// 2nd dynamic control point
	vec2 fourth_ap1 = vec2(0.5, 0.25*cos(4.0*uTime)+0.5);
	radius = distance(fourth_ap1, vec2(x, y));
	w[0] = smoothstep(0.0045, 0.006, radius);
	fColor += vec4(vec2(0.0), 1.0 - w[0].r, 0.0);

	// 3rd dynamic control point
	vec2 fourth_ap2 = vec2(0.75, -0.25*sin(4.0*uTime)+0.5);
	radius = distance(fourth_ap2, vec2(x, y));
	w[0] = smoothstep(0.0045, 0.006, radius);
	fColor += vec4(vec2(0.0), 1.0 - w[0].r, 0.0);

	fc = FourthOrderBezier(fourth_ap0, fourth_ap1, fourth_ap2, x);

	w[0] = smoothstep(fc-0.01, fc, y);
	w[1] = smoothstep(fc, fc+0.01, y);
	fColor += vec4(vec2(0.0), w[0]-w[1], 0.0);
}
