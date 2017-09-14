#version 450 core

#define subdivision	8

layout (triangles) in;

layout (triangle_strip, max_vertices = 256) out;

uniform mat4 uMVP;

void main()
{
	vec4 v0 = gl_in[0].gl_Position;
	vec4 v1 = gl_in[1].gl_Position;
	vec4 v2 = gl_in[2].gl_Position;

	float deltax = (v2.x - v0.x) / subdivision;
	float deltay = (v1.y - v0.y) / subdivision;

	float x = v0.x;
	float y = v0.y;

#if 0
	for (int i = 0; i < subdivision*subdivision; i++) {

		gl_Position = uMVP * vec4(x, 0, y, 1);
		EmitVertex();

		gl_Position = uMVP * vec4(x, 0, y+deltay, 1);
		EmitVertex();

		gl_Position = uMVP * vec4(x+deltax, 0, y, 1);
		EmitVertex();

		gl_Position = uMVP * vec4(x+deltax, 0, y+deltay, 1);
		EmitVertex();

		EndPrimitive();

		x+=deltax;

		if ((i+1)%subdivision == 0) {
			x=v0.x;
			y+=deltay;
		}
	}
#else
	for (int i = 0; i < subdivision; i++) {
		for (int j = 0; j < subdivision; j++) {

			gl_Position = uMVP * vec4(x+j*deltax, 0, y+i*deltay, 1);
			EmitVertex();

			gl_Position = uMVP * vec4(x+j*deltax, 0, y+(i+1)*deltay, 1);
			EmitVertex();

			gl_Position = uMVP * vec4(x+(j+1)*deltax, 0, y+i*deltay, 1);
			EmitVertex();

			gl_Position = uMVP * vec4(x+(j+1)*deltax, 0, y+(i+1)*deltay, 1);
			EmitVertex();

			EndPrimitive();

		}
	}
#endif
}
