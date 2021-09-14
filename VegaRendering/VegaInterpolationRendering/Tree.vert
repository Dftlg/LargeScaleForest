#version 430 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec2 aTexCoords;
layout (location = 3) in int faceId;
layout (location = 4) in mat4 instanceMatrix;
layout (location = 8 ) in int positionIndex;
//layout (location = 9) in mat4 specificRotationMatrix;


out vec2 v2f_TexCoords;
out vec3 v3f_WorldPos;
out vec3 v3f_Normal;

uniform int frameNums;
uniform int vertexNums;
uniform int assimpvertexNums;
uniform mat4 projection;
uniform mat4 view;
uniform mat4 model;

layout (std430, binding=1) buffer DeltaDeformationArray
{
	vec4 u[];
};

layout (std430, binding=2) buffer DeformationArray
{
	vec4 sum_u[];
};

layout (std430, binding=3) buffer IndexArray
{
	ivec2 treeFrameIndex[];
};

void main()
{

	v2f_TexCoords = aTexCoords; 
	v3f_WorldPos = vec3(model * vec4(aPos,1.0));
	v3f_Normal = mat3(model) *mat3(instanceMatrix)* aNormal;

	sum_u[gl_InstanceID*assimpvertexNums+positionIndex]=u[treeFrameIndex[gl_InstanceID][0]*frameNums*vertexNums+treeFrameIndex[gl_InstanceID][1]*vertexNums+faceId]+sum_u[gl_InstanceID*assimpvertexNums+positionIndex];

	vec4 tempPos=vec4(aPos,1.0)+sum_u[gl_InstanceID*assimpvertexNums+positionIndex];
	gl_Position = projection * view * model * instanceMatrix * tempPos;
}
