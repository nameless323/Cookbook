#version 430 core

in vec3 LightIntensity;

out vec4 col;

void main(void)
{
	col = vec4(LightIntensity, 1.0);
}