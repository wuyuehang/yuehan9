#version 450

layout (location = 0) in vec4 vin_Pos;
//layout (location = 1) in vec3 vin_Norm;

//out vec3 fin_Norm;

uniform mat4 uModel;
uniform mat4 uView;
uniform mat4 uProj;

void main()
{
	gl_Position = uProj * uView * uModel * vin_Pos;
	//gl_Position = uProj * uModel * vin_Pos;
	//fin_Norm = vin_Norm;
}
