#version 430
 
out vec4 outputF;


uniform float time;


void main()
{

	vec3 color = vec3(0.5);
	
	outputF = vec4(color, 1.0);
	
 }