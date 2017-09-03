#version 450

#define PI	3.141592657589793
precision highp float;

uniform uvec2 uResInfo;
uniform float uTime;

out vec4 fColor;

void main()
{
	// normalize
	float x = gl_FragCoord.x / uResInfo.x;
	float y = gl_FragCoord.y / uResInfo.y;

#if 0
	// No.1 linear transform
	fColor = vec4(y-x, x-y, 0.0, 0.0);
	fColor = vec4(1.0-y-x, -1.0+x+y, 0.0, 0.0);
#endif

#if 0
	// No.2 hermite interpolate
	// float t = clamp((x - edge0) / (edge1 - edge0), 0.0, 1.0);
	// return t * t * (3.0 - 2.0 * t);
	float up_weight = smoothstep(x-0.05, x, y);
	float down_weight = smoothstep(x, x+0.05, y);
	fColor = vec4(up_weight - down_weight, vec3(0.0));
#endif

#if 0
	// No.3 sine wave
	// control amplifier coefficient in [0, 0.5]
	float amplifier = 0.25*cos(2.5*uTime)+0.25;

	// control final result in [0, 1]
	float y1 = amplifier*sin(20.0*x+5.0*uTime)+0.5;
	float up_weight = smoothstep(y1-0.02, y1, y);
	float down_weight = smoothstep(y1, y1+0.02, y);
	fColor = vec4(up_weight - down_weight, vec3(0.0));
#endif

#if 0
	// No.4 tri sine wave with same amplifier and frequency
	// control amplifier coefficient in [0, 0.5]
	float amplifier[3];
	amplifier[0] = 0.25*cos(2.5*uTime)+0.25;

	// control final result in [0, 1]
	float wave[3];

	// 1st wave
	wave[0] = amplifier[0] * sin(20.0*x+5.0*uTime) + 0.5;
	float up_weight = smoothstep(wave[0]-0.02, wave[0], y);
	float down_weight = smoothstep(wave[0], wave[0]+0.02, y);

	fColor = vec4(up_weight-down_weight, vec3(0.0));

	// 2nd wave
	wave[1] = amplifier[0] * cos(20.0*x+5.0*uTime) + 0.5;
	up_weight = smoothstep(wave[1]-0.02, wave[1], y);
	down_weight = smoothstep(wave[1], wave[1]+0.02, y);

	fColor += vec4(0.0, up_weight-down_weight, vec2(0.0));

	// 3rd wave
	wave[2] = amplifier[0] * sin(20.*x+5.0*uTime+PI) + 0.5;
	up_weight = smoothstep(wave[2]-0.02, wave[2], y);
	down_weight = smoothstep(wave[2], wave[2]+0.02, y);

	fColor += vec4(vec2(0.0), up_weight-down_weight, 0.0);
#endif

#if 0
	// No.5 tri sine wave with differ amplifier and frequency
	// control amplifier coefficient in [0, 0.5]
	float amplifier[3];
	amplifier[0] = 0.25*cos(1.5*uTime)+0.25;
	amplifier[1] = 0.25*sin(3.*uTime)+0.25;
	amplifier[2] = 0.25*cos(6.*uTime+PI)+0.25;

	// control final result in [0, 1]
	float wave[3];

	// 1st wave
	wave[0] = amplifier[0] * sin(20.0*x+5.0*uTime) + 0.5;
	float up_weight = smoothstep(wave[0]-0.02, wave[0], y);
	float down_weight = smoothstep(wave[0], wave[0]+0.02, y);

	fColor = vec4(up_weight-down_weight, vec3(0.0));

	// 2nd wave
	wave[1] = amplifier[1] * cos(20.0*x+5.0*uTime) + 0.5;
	up_weight = smoothstep(wave[1]-0.02, wave[1], y);
	down_weight = smoothstep(wave[1], wave[1]+0.02, y);

	fColor += vec4(0.0, up_weight-down_weight, vec2(0.0));

	// 3rd wave
	wave[2] = amplifier[2] * sin(20.*x+5.0*uTime+PI) + 0.5;
	up_weight = smoothstep(wave[2]-0.02, wave[2], y);
	down_weight = smoothstep(wave[2], wave[2]+0.02, y);

	fColor += vec4(vec2(0.0), up_weight-down_weight, 0.0);
#endif

#if 1
	// No.5 multiple sine wave with differ frequency stack in the same channel

	float wave[3];

	wave[0] = 0.05 * sin(.5*x+5.0*uTime)
		+ 0.05 * sin(1.*x+5.0*uTime)
		+ 0.05 * sin(2.*x+5.0*uTime)
		+ 0.05 * sin(4.*x+5.0*uTime)
		+ 0.05 * sin(8.*x+5.0*uTime)
		+ 0.05 * sin(16.*x+5.0*uTime)
		+ 0.05 * sin(32.*x+5.0*uTime)
		+ 0.05 * sin(64.*x+5.0*uTime)
		+ 0.05 * sin(128.*x+5.0*uTime)
		+ 0.05 * sin(256.*x+5.0*uTime)
		+ 0.5;

	float up_weight = smoothstep(wave[0]-0.02, wave[0], y);
	float down_weight = smoothstep(wave[0], wave[0]+0.02, y);

	fColor = vec4(up_weight-down_weight, vec3(0.0));

	wave[1] = 0.05 * cos(.5*x+5.0*uTime)
		+ 0.05 * cos(1.*x+5.0*uTime)
		+ 0.05 * cos(2.*x+5.0*uTime)
		+ 0.05 * cos(4.*x+5.0*uTime)
		+ 0.05 * cos(8.*x+5.0*uTime)
		+ 0.05 * cos(16.*x+5.0*uTime)
		+ 0.05 * cos(32.*x+5.0*uTime)
		+ 0.05 * cos(64.*x+5.0*uTime)
		+ 0.05 * cos(128.*x+5.0*uTime)
		+ 0.05 * cos(256.*x+5.0*uTime)
		+ 0.5;

	up_weight = smoothstep(wave[1]-0.02, wave[1], y);
	down_weight = smoothstep(wave[1], wave[1]+0.02, y);

	fColor += vec4(0.0, up_weight-down_weight, vec2(0.0));

	wave[2] = 0.05 * sin(.5*x+5.0*uTime+PI)
		+ 0.05 * sin(1.*x+5.0*uTime+PI)
		+ 0.05 * sin(2.*x+5.0*uTime+PI)
		+ 0.05 * sin(4.*x+5.0*uTime+PI)
		+ 0.05 * sin(8.*x+5.0*uTime+PI)
		+ 0.05 * sin(16.*x+5.0*uTime+PI)
		+ 0.05 * sin(32.*x+5.0*uTime+PI)
		+ 0.05 * sin(64.*x+5.0*uTime+PI)
		+ 0.05 * sin(128.*x+5.0*uTime+PI)
		+ 0.05 * sin(256.*x+5.0*uTime+PI)
		+ 0.5;

	up_weight = smoothstep(wave[2]-0.02, wave[2], y);
	down_weight = smoothstep(wave[2], wave[2]+0.02, y);

	fColor += vec4(0.0, 0.0, up_weight-down_weight, 0.0);
#endif

#if 0
	// No.6 jigsaw wave
	float y1 = mod(x, 0.5);
	float up_weight = smoothstep(y1-0.02, y1, y);
	float down_weight = smoothstep(y1, y1+0.02, y);
	//fColor = vec4(up_weight-down_weight, vec3(0.0));
	fColor = vec4(up_weight, vec3(0.0));
#endif
}
