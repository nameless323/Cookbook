#version 430 core
in vec3 vcolor;
out vec4 col;

void main(void)
{
	col = vec4(vcolor, 1.0);
}