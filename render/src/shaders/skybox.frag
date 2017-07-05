#version 450

in vec3 fin_CubeCoord;

out vec4 fout_Color;

uniform samplerCube uCubeTexture;

void main()
{
	fout_Color = texture(uCubeTexture, fin_CubeCoord);
}
