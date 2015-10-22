#version 430 core

in vec3 vertexPosition;
in vec3 vertexColor;

out vec3 color;

void main(void)
{
	gl_Position = vec4(vertexPosition, 1.0);
	color = vertexColor;
}