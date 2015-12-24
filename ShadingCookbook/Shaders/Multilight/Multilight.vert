#version 430 core

layout (location = 0) in vec3 VertexPosition;
layout (location = 1) in vec3 VertexNormal;

out vec3 Color;

struct LightInfo
{
	vec4 Position;
	vec3 Intensity;
};

uniform LightInfo lights[5];

uniform vec3 Ka;
uniform vec3 Kd;
uniform vec3 Ks;
uniform float Shininess;

uniform mat4 ModelViewMatrix;
uniform mat3 NormalMatrix;
uniform mat4 ProjectionMatrix;
uniform mat4 MVP;


vec3 PhongModel(int lightIndex, vec4 pos, vec3 norm)
{
	vec3 s = normalize(vec3(lights[lightIndex].Position - pos));
	vec3 v = normalize(-pos.xyz);
	vec3 r = reflect(-s, norm);
	vec3 i = lights[lightIndex].Intensity;
	vec3 color = i * (Ka +
		Kd * max(dot(s, norm), 0.0) +
		Ks * pow(max(dot(r, v), 0.0), Shininess));
	return color;
}

void main(void)
{
	vec3 eyeNorm = normalize(NormalMatrix * VertexNormal);
	
	vec4 eyePos = ModelViewMatrix * vec4(VertexPosition, 1.0);

	Color = vec3(0.0);
	for (int i = 0; i < 5; i++)
		Color += PhongModel(i, eyePos, eyeNorm);
	gl_Position = MVP * vec4(VertexPosition, 1.0);
}