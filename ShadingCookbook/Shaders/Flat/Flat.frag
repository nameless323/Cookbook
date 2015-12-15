#version 430 core

flat in vec4 LightIntensity;
out vec4 col;

void main(void)
{
	col = LightIntensity;
}