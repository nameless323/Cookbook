#version 430 core

uniform vec4 LineColor;

layout (location = 0) out vec4 FragColor;

void main()
{
	FragColor = LineColor;
}