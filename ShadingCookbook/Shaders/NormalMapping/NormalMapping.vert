#version 430 core

layout (location = 0) in vec3 VertexPosition;
layout (location = 1) in vec3 VertexNormal;
layout (location = 2) in vec2 VertexTexCoord;
layout (location = 3) in vec4 VertexTangent;

struct LightInfo
{
	vec4 Position;
	vec3 Intensity;
};
uniform LightInfo Light;

out vec3 LightDir;
out vec2 TexCoord;
out vec3 ViewDir;

uniform mat4 ModelViewMatrix;
uniform mat3 NormalMatrix;
uniform mat4 ProjectionMatrix;
uniform mat4 MVP;

void main()
{
	vec3 norm = normalize(NormalMatrix * VertexNormal);
	vec3 tang = normalize(NormalMatrix * VertexTangent.xyz);
	vec3 binormal = normalize (cross(norm, tang)) * VertexTangent.w;

	mat3 toObjLocal = mat3(
		tang.x, binormal.x, norm.x,
		tang.y, binormal.y, norm.y,
		tang.z, binormal.z, norm.z
		);

	vec3 pos = vec3(ModelViewMatrix * vec4(VertexPosition, 1.0));
	LightDir = normalize(toObjLocal * (Light.Position.xyz - pos));
	ViewDir = toObjLocal * normalize(-pos);
	TexCoord = VertexTexCoord;

	gl_Position = MVP * vec4(VertexPosition, 1.0);
}