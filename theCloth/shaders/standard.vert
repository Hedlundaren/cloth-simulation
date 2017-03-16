#version 430
 
in vec4 position;
 
uniform float time;

uniform mat4 MV;
uniform mat4 P;

void main()
{

    gl_Position = P * MV * position;
} 