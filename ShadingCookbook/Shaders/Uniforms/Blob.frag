#version 430 core
in vec3 texCoord;
layout(location = 0) out vec4 fragColor;

uniform (binding = 0) BlobSettings
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
	fragColor = min(innerColor, outerColor, smoothstep(radiusInner, radiusOuter, dist));

	fragColor = vec4(0.0, 1.0, 0.0, 1.0);
}