#version 430 core

layout (location = 0) in vec3 VertexPosition;
layout (location = 1) in vec3 VertexNormal;

out vec3 FrontColor;
out vec3 BackColor;

struct LightInfo
{
	vec4 Position;
	vec3 La;
	vec3 Ld;
	vec3 Ls;
};

uniform LightInfo Light;

struct MaterialInfo
{
	vec3 Ka;
	vec3 Kd;
	vec3 Ks;
	float Shininess;
};

uniform MaterialInfo Material;

uniform mat4 ModelViewMatrix;
uniform mat3 NormalMatrix;
uniform mat4 ProjectionMatrix;
uniform mat4 MVP;

void GetEyeSpace(out vec4 pos, out vec3 norm)
{
	norm = normalize(NormalMatrix * VertexNormal);
	pos = ModelViewMatrix * vec4(VertexPosition, 1.0);
}

vec3 PhongModel(vec4 pos, vec3 norm)
{
	vec3 s = normalize(vec3(Light.Position - pos));
	vec3 v = normalize(-pos.xyz);
	vec3 r = reflect(-s, norm);
	vec3 ambient = Light.La * Material.Ka;
	float sDotN = max(dot(s, norm), 0.0);
	vec3 diffuse = Light.Ld * Material.Kd * sDotN;
	vec3 spec = vec3(0.0);
	if (sDotN > 0.0)
		spec = Light.Ls * Material.Ks * pow(max(dot(r, v), 0.0), Material.Shininess);
	return ambient + diffuse + spec;
}

void main(void)
{
	vec3 eyeNorm;
	vec4 eyePos;

	GetEyeSpace(eyePos, eyeNorm);
	FrontColor = PhongModel(eyePos, eyeNorm);
	BackColor = PhongModel(eyePos, -eyeNorm);
	//FrontColor = vec3(1.0);
	//BackColor = vec3(1.0);
	gl_Position = MVP * vec4(VertexPosition, 1.0);
}