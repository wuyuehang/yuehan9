#version 450

in vec3 fin_CubeCoord;
layout (early_fragment_tests) in;

out vec4 fout_Color;

uniform samplerCube uCubeTexture;

void main()
{
	fout_Color = texture(uCubeTexture, fin_CubeCoord);
}
