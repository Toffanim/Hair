#version 410 core
in vec3 TexCoords;
in vec4 pos;
out vec4 color;

uniform samplerCube skybox;
uniform sampler2D DepthBuffer;


void main()
{
	float screenWidth = 800.f;
	float screenHeight = 600.f;
	// Converting (x,y,z) to range [0,1]
	float x = gl_FragCoord.x/screenWidth;
	float y = gl_FragCoord.y/screenHeight;
	float z = gl_FragCoord.z; // Already in range [0,1]


    float depth = texture(DepthBuffer, vec2(x, y)).r;
    if ( depth >= 0.999 )
    {
             color = texture(skybox, TexCoords);    
    }
}
