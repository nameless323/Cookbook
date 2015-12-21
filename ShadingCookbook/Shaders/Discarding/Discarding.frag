#version 430 core

in vec3 FrontColor;
in vec3 BackColor;
in vec2 uv;

out vec4 col;

void main(void)
{
	const float scale = 15.0;
	bvec2 toDiscard = greaterThan (fract(uv*scale), vec2(0.2, 0.2));
	
	if (all(toDiscard))
		discard;
	if (gl_FrontFacing)
		col = vec4(FrontColor, 1.0);
	else
		col = vec4(BackColor, 1.0);
}