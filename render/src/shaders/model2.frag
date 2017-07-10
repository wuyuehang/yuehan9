#version 450

precision mediump float;

uniform vec3 uCameraLoc;
uniform samplerCube uCubeTexture;
uniform float uRefractionRatio;
uniform int uIsRefractionMode;

in vec3 fNormal;
in vec3 fWorld;

out vec4 fColor;

void main()
{
	vec3 view_dir = normalize(fWorld - uCameraLoc);
	vec3 texcoord;

	if (bool(uIsRefractionMode)) {
		texcoord = refract(view_dir, normalize(fNormal), uRefractionRatio);
	} else {
		texcoord = reflect(view_dir, normalize(fNormal));
	}

	fColor = texture(uCubeTexture, texcoord);
}
