#version 410 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec2 aTexCoords;
layout (location = 3) in vec3 aDeformation;
//layout (location = 3) in mat4 aInstanceMatrix;

out vec2 TexCoords;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

void main()
{
    TexCoords = aTexCoords;   
	vec3 tempPos=aPos+aDeformation;
    gl_Position = projection * view * model * vec4(tempPos, 1.0);
	
}