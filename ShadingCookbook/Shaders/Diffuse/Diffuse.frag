in vec3 LightIntensity;

layout (location = 0) out vec4 col;

void main(void)
{
	col = vec4(LightIntensity);
}