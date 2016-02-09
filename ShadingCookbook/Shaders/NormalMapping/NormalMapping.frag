#version 430 core

in vec3 LightDir;
in vec2 TexCoord;
in vec3 ViewDir;

layout (binding = 0) uniform sampler2D ColorTex;
layout (binding = 1) uniform sampler2D NormalMapTex;

struct LightInfo
{
	vec4 Position;
	vec3 Intensity;
};
uniform LightInfo Light;

struct MaterialInfo
{
	vec3 Ka;
	vec3 Ks;
	float Shininess;
};
uniform MaterialInfo Material;

layout (location = 0) out vec4 FragColor;

vec3 phongModel(vec3 norm, vec3 diffR)
{
	vec3 r = reflect(-LightDir, norm);
	vec3 ambient = Light.Intensity * Material.Ka;
	float sDotN = max(dot(LightDir, norm), 0.0);
	vec3 diffuse = Light.Intensity * diffR * sDotN;

	vec3 spec = vec3(0.0);
	if (sDotN > 0.0)
	{
		spec = Light.Intensity * Material.Ks * pow (max (dot(r, ViewDir), 0.0), Material.Shininess);
	}
	return ambient + diffuse + spec;
}

void main(void)
{
	vec4 normal = 2.0 * texture(NormalMapTex, TexCoord) - 1.0;

	vec4 texColor = texture(ColorTex, TexCoord);
	FragColor = vec4(phongModel(normal.xyz, texColor.rgb), 1.0);
}