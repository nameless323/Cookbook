#version 430 core

struct LightInfo
{
	vec4 Position;
	vec3 Intensity;
};
uniform LightInfo Light;

struct MaterialInfo
{
	vec3 Kd;
};
uniform MaterialInfo Material;

subroutine void RenderPassType();
subroutine uniform RenderPassType RenderPass;

layout (binding = 0) uniform sampler2D PositionTex;
layout (binding = 1) uniform sampler2D NormalTex;
layout (binding = 2) uniform sampler2D ColorTex;

in vec3 Position;
in vec3 Normal;
in vec2 TexCoord;

layout (location = 0) out vec4 FragColor;
layout (location = 1) out vec3 PositionData;
layout (location = 2) out vec3 NormalData;
layout (location = 3) out vec3 ColorData;

vec3 diffuseModel (vec3 pos, vec3 norm, vec3 diff)
{
	vec3 s = normalize(vec3(Light.Position) - pos);
	float sDotN = max(dot(s, norm), 0.0);
	vec3 diffuse = Light.Intensity * diff * sDotN;

	return diffuse;
}

subroutine(RenderPassType)
void pass1 ()
{
	PositionData = Position;
	NormalData = Normal;
	ColorData = Material.Kd;
}

subroutine(RenderPassType)
void pass2 ()
{
	vec3 pos = vec3(texture(PositionTex, TexCoord));
	vec3 norm = vec3(texture(NormalTex, TexCoord));
	vec3 diffColor = vec3(texture(ColorTex, TexCoord));

	FragColor = vec4 (diffuseModel(pos, norm, diffColor), 1.0);
}

void main()
{
	RenderPass();
}