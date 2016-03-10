#version 410 core

in block
{
	vec2 Texcoord;
} In; 

uniform sampler2D Texture;

layout(location = 0, index = 0) out vec4  Color;


//  http://filmicgames.com/archives/75
// http://frictionalgames.blogspot.fr/2013/11/tech-feature-linear-space-lighting.html
float A = 0.15; 
float B = 0.50;
float C = 0.10;
float D = 0.20;
float E = 0.02;
float F = 0.30;
float W = 11.2;

vec3 toneMapping( vec3 x )
{
    return ((x*(A*x+C*B)+D*E) / (x*(A*x+B)+D*F)) - E/F ;
}

void main()
{ 
    const float exposureAdjustment = 8.0; 
    const float gamma = 2.2;
    const float exposureBias = 0.5;   

    vec3 texColor = texture(Texture, In.Texcoord).rgb;
    texColor *= exposureAdjustment;

    vec3 curr = toneMapping( exposureBias * texColor );
    vec3 whiteScale = 1.0f / toneMapping( vec3(W) );
    vec3 mapped = curr*whiteScale;
    // Gamma correction 
    mapped = pow(mapped, vec3(1.0 / gamma));
  
    Color = vec4( mapped, 1.0);
} 
