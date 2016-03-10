#version 410 core

in block
{
	vec2 Texcoord;
} In; 

uniform sampler2D scene;
uniform sampler2D bloomBlur;

layout(location = 0, index = 0) out vec4  Color;


void main()
{   

    vec3 texColor = texture(scene, In.Texcoord).rgb;
    texColor += texture(bloomBlur, In.Texcoord).rgb;
  
    Color = vec4( texColor, 1.0);
} 
