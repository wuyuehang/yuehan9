#version 450

/* used by pixel_karate demo */
#define SEL_TOPLEFT	1
#define SEL_BOTLEFT	1
#define SEL_TOPRIGH	1
#define SEL_BOTRIGH	1

precision highp float;

uniform uvec2 uResInfo;
uniform float uTime;
uniform vec2 uMouse;

out vec4 fColor;

#define T0	0.005
#define T1	0.015

void main()
{
	float w[2];
	float x = gl_FragCoord.x / uResInfo.x;
	float y = gl_FragCoord.y / uResInfo.y;
	float fc;
	fColor = vec4(0.0);
	x = 2.0*x-1.0;
	y = 2.0*y-1.0;

	int i;
	float degree = 0.01;
	float inner_radius;
	float layout_amplifier;
	float illumi_amplifier;

	for (i = 0; i < 100; i++) {

		// common components
		layout_amplifier = 0.75*cos(0.01*float(i+1)*uTime)+0.75;
		illumi_amplifier = pow(degree*float(i), 2);
		inner_radius = 0.05*sin(uTime)+0.05;

#if SEL_BOTLEFT
		// bottom-left
		fc = layout_amplifier*((x+0.5)*(x+0.5) + (y+0.5)*(y+0.5));
		w[0] = smoothstep(inner_radius, inner_radius+T1, fc);
		w[1] = smoothstep(inner_radius+T0, inner_radius+T1, fc);
		fColor += illumi_amplifier*vec4(w[0]-w[1], vec3(0.0));
#endif

#if SEL_BOTRIGH
		// bottom-right
		fc = layout_amplifier*((x-0.5)*(x-0.5) + (y+0.5)*(y+0.5));
		w[0] = smoothstep(inner_radius, inner_radius+T1, fc);
		w[1] = smoothstep(inner_radius+T0, inner_radius+T1, fc);
		fColor += illumi_amplifier*vec4(0.0, w[0]-w[1], vec2(0.0));
#endif

#if SEL_TOPRIGH
		// top-right
		fc = layout_amplifier*((x-0.5)*(x-0.5) + (y-0.5)*(y-0.5));
		w[0] = smoothstep(inner_radius, inner_radius+T1, fc);
		w[1] = smoothstep(inner_radius+T0, inner_radius+T1, fc);
		fColor += illumi_amplifier*vec4(0.0, 0.0, w[0]-w[1], 0.0);
#endif

#if SEL_TOPLEFT
		// top-left
		fc = layout_amplifier*((x+0.5)*(x+0.5) + (y-0.5)*(y-0.5));
		w[0] = smoothstep(inner_radius, inner_radius+T1, fc);
		w[1] = smoothstep(inner_radius+T0, inner_radius+T1, fc);
		fColor += illumi_amplifier*vec4(0.0, vec2(w[0]-w[1]), 0.0);
#endif

		// middle
#if 0
		fc = layout_amplifier*((x-uMouse.x)*(x-uMouse.x) + (y+uMouse.y)*(y+uMouse.y));
		w[0] = smoothstep(inner_radius, inner_radius+T1, fc);
		w[1] = smoothstep(inner_radius+T0, inner_radius+T1, fc);
		fColor += illumi_amplifier*vec4(vec2(w[0]-w[1]), vec2(0.0));
#else
		fc = layout_amplifier*(x*x + y*y);
		w[0] = smoothstep(inner_radius, inner_radius+T1, fc);
		w[1] = smoothstep(inner_radius+T0, inner_radius+T1, fc);
		fColor += illumi_amplifier*vec4(vec2(w[0]-w[1]), vec2(0.0));
#endif
	}
}
