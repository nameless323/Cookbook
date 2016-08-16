#version 430 core

in float Transp;
uniform sampler2D ParticleTex;

layout (location = 0) out vec4 FragColor;

void main()
{
	FragColor = texture (ParticleTex, gl_PointCoord);
	FragColor = vec4(0.0, 0.4, 0.7, 1.0);
	FragColor.a *= Transp;
}