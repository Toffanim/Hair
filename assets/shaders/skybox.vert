#version 410 core
layout (location = 0) in vec3 position;
out vec3 TexCoords;
out vec4 pos;

uniform mat4 projection;
uniform mat4 view;


void main()
{
    gl_Position = projection * view * vec4(position, 1.0); 
    pos = gl_Position; 
    TexCoords = position;
} 
