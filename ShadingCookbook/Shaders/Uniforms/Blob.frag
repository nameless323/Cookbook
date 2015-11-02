#version 430 core
in vec2 texCoord;
layout(location = 0) out vec4 fragColor;

layout (std140, binding = 0) uniform BlobSettings
{
	vec4 innerColor;
	vec4 outerColor;
	float radiusInner;
	float radiusOuter;
};

void main(void)
{
	float dx = texCoord.x - 0.5;
	float dy = texCoord.y - 0.5;
	float dist = sqrt(dx*dx + dy*dy);
	fragColor = mix(innerColor, outerColor, smoothstep(radiusInner, radiusOuter, dist));
}