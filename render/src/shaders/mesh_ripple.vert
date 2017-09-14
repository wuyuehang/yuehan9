#version 450 core

layout (location = 0) in vec4 vPos;

uniform float uTime;
uniform mat4 uMVP;

void main()
{
	float amplifier[2];

	amplifier[0] = 25.0;

	float frequency = amplifier[0]*length(vPos);

	amplifier[1] = 0.125 / length(vPos);

	float y = amplifier[1]*sin(frequency+uTime);

	vec4 world = vec4(vPos.x, y, vPos.y, 1.0);

	gl_Position = uMVP * world;
}
