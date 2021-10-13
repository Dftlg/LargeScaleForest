#version 430 core
out vec4 FragColor;


in vec2 v2f_TexCoords;
in vec3 v2f_WorldPos;
in vec3 v2f_Normal;

// material parameters
//uniform vec3 albedo;
uniform float metallic;
uniform float roughness;
uniform float ao;

// lights
uniform vec3 lightPositions;
uniform vec3 lightColors;

uniform vec3 camPos;
uniform vec3 viewPos;

uniform float far_plane;
uniform bool shadows;

uniform sampler2D texture_diffuse1;

uniform mat4 lightspaceMatrix;

void main()
{             
	FragColor = vec4(1.0,1.0,1.0, 1.0);
}
