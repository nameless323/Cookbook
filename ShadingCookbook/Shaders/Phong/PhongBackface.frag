#version 430 core

in vec3 BackColor;
in vec3 FrontColor;
out vec4 col;

void main(void)
{
	col = mix(vec4(BackColor, 1.0), vec4(FrontColor, 1.0), gl_FrontFacing);
}