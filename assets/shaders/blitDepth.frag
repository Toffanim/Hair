#version 410 core

in block
{
	vec2 Texcoord;
} In; 

uniform float near_plane = 0.1f;
uniform float far_plane = 10.0f;
uniform sampler2D Texture;

layout(location = 0, index = 0) out vec4  Color;

float LinearizeDepth(float depth)
{
    float z = depth * 2.0 - 1.0; // Back to NDC 
    return (2.0 * near_plane * far_plane) / (far_plane + near_plane - z * (far_plane - near_plane));
}

void main(void)
{
	float depth = texture(Texture, In.Texcoord).r;
        
        //if ( color.x > 1.0 || color.y > 1.0 || color.z > 1.0)
	Color = vec4( vec3(LinearizeDepth(depth)/far_plane), 1.0 );
}
