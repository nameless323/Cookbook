#version 430 core

layout(location = 0) in vec3 vertexPosition;

layout(location = 1) in vec3 vertexColor;

layout(location = 2) uniform mat4 rotMat;

out vec3 vcolor;

void main(void)
{
	gl_Position = rotMat * vec4(vertexPosition, 1.0);
	vcolor = vertexColor;
}