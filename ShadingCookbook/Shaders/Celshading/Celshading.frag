#version 430 core

in vec3 Position;
in vec3 Normal;

struct LightInfo
{
	vec4 position;
	vec3 intensity;
};

uniform LightInfo Light;

uniform vec3 Kd;
uniform vec3 Ka;

const int levels = 3;
const float scaleFactor = 1.0/levels;

layout (location = 0) out vec4 FragColor;

vec3 ToonShade()
{
	vec3 s = normalize(Light.position.xyz - Position.xyz);
	float cosine = max(0.0, dot(s, Normal));
	vec3 diffuse = Kd * floor(cosine * levels) * scaleFactor;
	return Light.intensity * (Ka + diffuse);
}

void main()
{
	FragColor = vec4(ToonShade(), 1.0);
}