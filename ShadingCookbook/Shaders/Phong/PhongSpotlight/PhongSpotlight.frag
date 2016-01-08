#version 430 core

in vec3 Position;
in vec3 Normal;

struct SpotLightInfo
{
	vec4 position;
	vec3 intensity;
	vec3 direction;
	float exponent;
	float cutoff;
};
uniform SpotLightInfo Spot;

uniform vec3 Kd;
uniform vec3 Ka;
uniform vec3 Ks;
uniform float Shininess;

layout (location = 0) out vec4 FragColor;

vec3 adsWithSpotlight()
{
	vec3 s = normalize(vec3(Spot.position) - Position);
	vec3 spotDir = normalize(Spot.direction);
	float angle = acos(dot(-s, spotDir));
	float cutoff = radians(clamp(Spot.cutoff, 0.0, 90.0));
	vec3 ambient = Spot.intensity * Ka;

	if (angle < cutoff)
	{
		float spotFactor = pow(dot(-s, spotDir), Spot.exponent);
		vec3 v = normalize(vec3(-Position));
		vec3 h = normalize(v + s);
		return ambient + spotFactor * Spot.intensity * (Kd * max(dot(s, Normal), 0.0) + Ks * pow(max(dot(h, Normal), 0.0), Shininess));
	}
	else
	{
		return ambient;
	}
}

void main()
{
	FragColor = vec4(adsWithSpotlight(), 1.0);
}