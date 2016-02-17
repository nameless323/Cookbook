#version 430 core

in vec3 ReflectDir;
in vec3 RefractDir;

layout (binding = 0) uniform samplerCube CubeMapTex;

uniform bool DrawSkyBox;

struct MaterialInfo
{
	float Eta;
	float ReflectionFactor;
};

uniform MaterialInfo Material;

layout (location = 0) out vec4 FragColor;

void main()
{
	vec4 reflectColor = texture(CubeMapTex, ReflectDir);
	vec4 refractColor = texture(CubeMapTex, RefractDir);
	if (DrawSkyBox)
		FragColor = reflectColor;
	else
		FragColor = mix(refractColor, reflectColor, Material.ReflectionFactor);
}