#version 450 core

precision highp float;

uniform int uSampleIndex;

out vec4 fColor;

layout (binding = 0) uniform sampler2DMS MS;

void main()
{
	fColor = texelFetch(MS, ivec2(gl_FragCoord.xy), uSampleIndex);
}
