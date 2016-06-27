#version 430 core

struct LightInfo
{
	vec4 Position;
	vec3 Intensity;
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

uniform vec4 LineColor;

in vec3 GPosition;
in vec3 GNormal;

flat in int GIsEdge;

layout (location = 0) out vec4 FragColor;

const int levels = 3;
const float scaleFactor = 1.0 / levels;

vec3 toonShade()
{
	vec3 s = normalize( Light.Position.xyz - GPosition.xyz );
    vec3 ambient = Material.Ka;
    float cosine = dot( s, GNormal );
    vec3 diffuse = Material.Kd * ceil( cosine * levels ) * scaleFactor;

    return Light.Intensity * (ambient + diffuse);
}

void main()
{
	FragColor = mix(vec4(toonShade(), 1.0), LineColor, GIsEdge);
}