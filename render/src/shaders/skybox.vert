#version 450

layout (location = 0) in vec4 vin_Pos;

out vec3 fin_CubeCoord;

uniform mat4 uView;
uniform mat4 uProj;

void main()
{
	vec4 raw_position = uProj * uView * vin_Pos;
	gl_Position = raw_position.xyww;
	fin_CubeCoord = vin_Pos.xyz;
}
