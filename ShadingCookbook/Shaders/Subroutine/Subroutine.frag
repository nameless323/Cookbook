#version 430 core

in vec4 LightIntensity;
out vec4 col;

void main(void)
{
	col = LightIntensity;
}