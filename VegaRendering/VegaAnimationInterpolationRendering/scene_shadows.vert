#version 430 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec2 aTexCoords;
layout (location = 3) in int faceId;
layout (location = 4) in mat4 instanceMatrix;
layout (location = 8 ) in int positionIndex;

out vec2 v2f_TexCoords;
out vec3 v2f_WorldPos;
out vec3 v2f_Normal;

uniform int frameNums;
uniform int vertexNums;
uniform mat4 projection;
uniform mat4 view;
uniform mat4 model;
uniform int frameIndex;
uniform int treeIndex;
uniform int time;

const float PI = 3.14159265359;

layout (std430, binding=1) buffer DeformationArray
{
	vec4 u[];
};

void main()
{
	v2f_TexCoords = aTexCoords; 
	//v2f_Normal = mat3(model) * aNormal;
	//v2f_WorldPos = vec3(model * vec4(aPos,1.0));

	vec4 tempPos;

	if(treeIndex < 0)
	{
		v2f_WorldPos = vec3(model * vec4(aPos,1.0));
		v2f_Normal = mat3(model) * aNormal;
		gl_Position = projection * view * model * vec4(aPos, 1.0);
		
	} 
	else
	{
		tempPos = vec4(aPos,1.0)+u[treeIndex * frameNums * vertexNums + frameIndex * vertexNums+faceId];
		v2f_WorldPos = vec3(model * instanceMatrix * tempPos);
		v2f_Normal = mat3(model) * mat3(instanceMatrix) * aNormal;	
	    gl_Position = projection * view * model * instanceMatrix * tempPos; 
	}

//	vec4 tempPos;
//	if(gl_InstanceID<5)
//	{
//		tempPos=vec4(aPos,1.0)+u[gl_InstanceID*frameNums*vertexNums+frameIndex*vertexNums+faceId];
//	}
//	else
//	{
//		int tempInstanceIndex= gl_InstanceID / 100;
//		tempPos=vec4(aPos,1.0)+u[tempInstanceIndex*frameNums*vertexNums+frameIndex*vertexNums+faceId];
//	}

	//when use gpuInstance
	//gl_Position = projection * view * model * instanceMatrix * tempPos;
//	if(faceId>=7422)
//	{
//		tempPos=tempPos*sin((2*M_PI/60)*time);
//	}
//
	
}
