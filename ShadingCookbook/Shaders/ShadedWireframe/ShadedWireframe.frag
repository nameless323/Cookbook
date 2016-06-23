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

uniform struct LineInfo 
{
  float Width;
  vec4 Color;
} Line;

in vec3 GPosition;
in vec3 GNormal;
noperspective in vec3 GEdgeDistance;

layout( location = 0 ) out vec4 FragColor;

vec3 phongModel( vec3 pos, vec3 norm )
{
    vec3 s = normalize(vec3(Light.Position) - pos);
    vec3 v = normalize(-pos.xyz);
    vec3 r = reflect( -s, norm );
    vec3 ambient = Light.Intensity * Material.Ka;
    float sDotN = max( dot(s,norm), 0.0 );
    vec3 diffuse = Light.Intensity * Material.Kd * sDotN;
    vec3 spec = vec3(0.0);
    if( sDotN > 0.0 )
        spec = Light.Intensity * Material.Ks *
               pow( max( dot(r,v), 0.0 ), Material.Shininess );

    return ambient + diffuse + spec;
}

void main()
{
	vec4 color = vec4(phongModel(GPosition, GNormal), 1.0);

	float d = min(GEdgeDistance.x, GEdgeDistance.y);
	d = min(d, GEdgeDistance.z);

	float mixVal;
	if (d < Line.Width - 1)
	{
		mixVal = 1.0;
	}
	else if (d > Line.Width + 1)
	{
		mixVal = 0.0;
	}
	else
	{
		float x = d - (Line.Width - 1);
		mixVal = exp2(-2.0 * (x * x));
	}
	FragColor = mix(color, Line.Color, mixVal);
}