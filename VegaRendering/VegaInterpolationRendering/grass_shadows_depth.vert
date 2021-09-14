#version 430 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec2 aTexCoords;
layout (location = 4) in mat4 instanceMatrix;

uniform mat4 model;

uniform mat4 lightSpaceMatrix;

void main()
{
	gl_Position =lightSpaceMatrix* model * instanceMatrix * vec4(aPos, 1.0);
	//gl_Position = model * vec4(aPos, 1.0);
}
