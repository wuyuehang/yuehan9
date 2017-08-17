#version 450 core

precision mediump float;

#define BACKGROUND_C	vec4(0.1, 0.15, 0.2, 0.0)
uniform _WRAP {
	uvec4 bbox;
	vec4 color;
	uvec2 resinfo;
} WRAP;

out vec4 FragColor;

void main()
{
	if (gl_FragCoord.x > WRAP.bbox.x && gl_FragCoord.x < WRAP.bbox.y
		&& gl_FragCoord.y > WRAP.bbox.z && gl_FragCoord.y < WRAP.bbox.w) {

		float dx = float(gl_FragCoord.x - WRAP.resinfo.x / 2);
		float dy = float(gl_FragCoord.y - WRAP.resinfo.y / 2);
		float dist = sqrt(dx*dx + dy*dy);

		if (dist < 100) {
			FragColor = mix(WRAP.color, BACKGROUND_C, smoothstep(10.0, 120.0, dist));
		} else {
			FragColor = vec4(0.1, 0.15, 0.2, 0.0);
		}
	} else {
		FragColor = vec4(0.0);
	}
}
