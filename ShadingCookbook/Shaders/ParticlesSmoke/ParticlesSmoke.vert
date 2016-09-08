#version 430 core

subroutine void RenderPassType();
subroutine uniform RenderPassType RenderPass;

layout (location = 0) in vec3 VertexPosition;
layout (location = 1) in vec3 VertexVelocity;
layout (location = 2) in float VertexStartTime;
layout (location = 3) in vec3 VertexInitialVelocity;

out vec3 Position;
out vec3 Velocity;
out float StartTime;
out float Transp;

uniform float Time;
uniform float H;
uniform vec3 Accel;
uniform float ParticleLifetime;
uniform float MinParticleSize = 10.0;
uniform float MaxParticleSize = 200.0;

uniform mat4 MVP;

subroutine (RenderPassType)
void update()
{
	Position = VertexPosition;
    Velocity = VertexVelocity;
    StartTime = VertexStartTime;

    if( Time >= StartTime ) 
    {

        float age = Time - StartTime;

        if( age > ParticleLifetime ) 
        {    
            Position = vec3(0.0);
            Velocity = VertexInitialVelocity;
            StartTime = Time;
        } 
        else 
        {        
            Position += Velocity * H;
            Velocity += Accel * H;
        }
    }
}

subroutine (RenderPassType)
void render()
{
	float age = Time - VertexStartTime;
	Transp = 0.0;
	if (Time >= VertexStartTime)
	{
		float agePct = age / ParticleLifetime;
		Transp = 1.0 - agePct;
	}
	gl_Position = MVP * vec4(VertexStartTime, 1.0);
}

void main()
{
	RenderPass();
}