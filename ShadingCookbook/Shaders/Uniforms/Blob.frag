#version 430 core
in vec3 texCoord;
layout(location = 0) out vec4 fragColor;

layout (std140, binding = 0) uniform BlobSettings
{
	vec4 innerColor;
	vec4 outerColor;
	float radiusInner;
	float radiusOuter;
} blob;

void main(void)
{
	float dx = texCoord.x - 0.5;
	float dy = texCoord.y - 0.5;
	float dist = sqrt(dx*dx + dy*dy);
	fragColor = mix(blob.innerColor, blob.outerColor, smoothstep(blob.radiusInner, blob.radiusOuter, dist));
}