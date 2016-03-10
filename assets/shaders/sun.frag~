#version 410 core

in block
{
	vec2 Texcoord;
} In; 

out vec4 color;

uniform sampler2D DepthBuffer;
uniform vec3 sun;


void main()
{
    float depth = texture(DepthBuffer, In.Texcoord).r;
    if ( depth >= 0.999 )
    {
        if ( distance( vec2(sun), In.Texcoord ) <= 0.05 )
             color = vec4( 2.0, 2.0, 2.0, 1.0);   
    }
}
