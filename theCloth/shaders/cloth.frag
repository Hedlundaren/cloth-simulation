#version 430
 

in vec3 newPos;
in vec3 newNormal;
in vec2 texCoord;

out vec4 outputF;


uniform float time;
uniform float camPos;
uniform sampler2D albinTexture;
uniform sampler2D carpetTexture;


void main()
{
	vec3 lightDir = normalize(vec3(-1, 0.2, 0.5));
	vec3 normal = newNormal;

	// Colors 
	vec3 ambient_color = vec3(0.0);
	vec3 diffuse_color = vec3(texture(carpetTexture, texCoord));
	vec3 specular_color = vec3(1.0);
	ambient_color = diffuse_color;

	// Diffuse
	float a = clamp( dot( normal, lightDir ), 0.0, 1.0);
    vec3 diffuse = 1.0 * a * diffuse_color;

	// Ambient
	float b = 0.1;
	vec3 ambient = ambient_color * b;

	// Specular
	float ks = 0.1;
	vec3 eye_pos = normalize(camPos - newPos);
	vec3 R = 2.0*dot(lightDir,normal)*normal - lightDir;
	vec3 specular = ks * pow( clamp(dot(R, eye_pos), 0.0, 1.0), 34.9) * specular_color;	

	vec3 color = diffuse + ambient + specular;


	outputF = vec4(color, 1.0);
	
 }