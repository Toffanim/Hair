#version 410 core

in block
{
	vec2 Texcoord;
} In; 

uniform sampler2D Texture;

layout(location = 0, index = 0) out vec4  Color;

void main(void)
{
	vec3 color = texture(Texture, In.Texcoord).rgb;
        float brightness = dot(color, vec3(0.2126, 0.7152, 0.0722));
        if( color.x > 1.0 || color.y > 1.0 || color.z > 1.0)
	    Color = vec4(color,1.0);
}
