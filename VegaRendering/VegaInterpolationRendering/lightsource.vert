#version 430 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec2 aTexCoords;
layout (location = 4) in mat4 instanceMatrix;

out vec2 v2f_TexCoords;
out vec3 v2f_WorldPos;
out vec3 v2f_Normal;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;
uniform mat4 lightSpaceMatrix;

void main()
{
    v2f_TexCoords = aTexCoords;
	v2f_Normal = mat3(model) * aNormal;
	v2f_WorldPos = vec3(model*instanceMatrix * vec4(aPos,1.0));
    gl_Position = projection * view *instanceMatrix* model * vec4(aPos, 1.0);
}
