#version 450

precision mediump float;

uniform vec3 uCameraLoc;
uniform samplerCube uCubeTexture;
uniform float uRefractionRatio;
uniform int uIsRefractionMode;

in vec3 fin_Norm;
in vec3 fin_World_Pos;

out vec4 fout_Color;

void main()
{
	vec3 view_direction = normalize(fin_World_Pos - uCameraLoc);
	vec3 texcoord;

	if (bool(uIsRefractionMode)) {
		texcoord = refract(view_direction, normalize(fin_Norm), uRefractionRatio);
	} else {
		texcoord = reflect(view_direction, normalize(fin_Norm));
	}

	fout_Color = texture(uCubeTexture, texcoord);
}
