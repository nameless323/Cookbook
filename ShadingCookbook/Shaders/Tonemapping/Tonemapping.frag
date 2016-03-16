#version 430 core

struct LightInfo
{
	vec4 Position;
	vec3 Intensity;
};
uniform LightInfo Lights[3];

struct MaterialInfo
{
	vec3 Ka;
	vec3 Kd;
	vec3 Ks;
	float Shine;
};
uniform MaterialInfo Material;

uniform float AveLum;

subroutine void RenderPassType();
subroutine uniform RenderPassType RenderPass;

layout (binding = 0) uniform sampler2D HdrTex;

in vec3 Position;
in vec3 Normal;
in vec2 TexCoord;

layout (location = 0) out vec4 FragColor;
layout (location = 1) out vec3 HdrColor;

// XYZ/RGB conversion matrices from:
// http://www.brucelindbloom.com/index.html?Eqn_RGB_XYZ_Matrix.html

uniform mat3 rgb2xyz = mat3
( 
	0.4124564, 0.2126729, 0.0193339,
	0.3575761, 0.7151522, 0.1191920,
	0.1804375, 0.0721750, 0.9503041 
);

uniform mat3 xyz2rgb = mat3
(
	3.2404542, -0.9692660, 0.0556434,
	-1.5371385, 1.8760108, -0.2040259,
	-0.4985314, 0.0415560, 1.0572252
);

uniform float Exposiure = 0.35;
uniform float White = 0.928;
uniform bool DoToneMap = true;

vec3 ads (vec3 pos, vec3 norm)
{
	vec3 v = normalize(vec3(-pos));
	vec3 total = vec3(0.0f, 0.0f, 0.0f);
	for (int i = 0; i < 3; i++)
	{
		vec3 s = normalize(vec3(Lights[i].Position) - pos);
		vec3 r = reflect(-s, norm);
		total += Lights[i].Intensity * (Material.Ka + Material.Kd * max(dot(s, norm), 0.0) + Material.Ks * pow (max(dot(r,v) 0.0), Material.Shine));
	}
	return total;
}

subroutine (RenderPassType)
void pass1()
{
	HdrColor = ads(Position, Normal);
}

subroutine (RenderPassType)
void pass2()
{
	vec4 color = texture(HdrTex, TexCoord);
	vec3 xyzCol = rgb2xyz * vec3(color);
	float xyzSum = xyzCol.x + xyzCol.y + xyzCol.z;
	vec3 xyYCol = vec3(xyzCol.x / xyzSum, xyzCol.y / xyzSum, xyzCol.y);

	float L = (Exposiure * xyYCol.z) / AveLum;
	L = (L * (1 - xyYCol.x - xyYCol.y)) / xyYCol.y;

	xyzCol.x = (L * xyYCol.x) / (xyYCol.y);
	xyzCol.y = L;
	yexCol.z = (L * (1 - xyYCol.x - xyYCol.y))/xyYCol.y;

	if (DoToneMap)
		FragColor = vec4(xyz2rgb * xyzCol, 1.0);
	else
		FragColor = color;	
}

void main()
{
	RenderPass();
}