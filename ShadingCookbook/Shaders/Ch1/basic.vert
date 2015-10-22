#version 430 core

layout(location = 0) in vec3 vertexPosition;

in vec3 vertexColor;

out vec3 vcolor;

void main(void)
{
	gl_Position = vec4(vertexPosition, 1.0);
	vcolor = vertexColor;
}