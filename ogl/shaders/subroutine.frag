#version 430 core

subroutine vec4 gen_const_color();
subroutine uniform gen_const_color gen_ptr;

subroutine (gen_const_color) vec4 gen_red()
{
	return vec4(1.0f, 0.0f, 0.0f, 1.0f);
}

subroutine (gen_const_color) vec4 gen_blue()
{
	return vec4(0.0f, 0.0f, 1.0f, 1.0f);
}

out vec4 color;

void main()
{
	color = gen_ptr();
}
