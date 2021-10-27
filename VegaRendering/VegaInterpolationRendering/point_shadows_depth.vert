#version 430 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec2 aTexCoords;
layout (location = 3) in int faceId;
layout (location = 4) in mat4 instanceMatrix;
layout (location = 8 ) in int positionIndex;

out vec4 testSumU;

uniform mat4 model;
uniform int assimpvertexNums;
uniform int planeOrTree;
uniform int frameNums;
uniform int frameIndex;
uniform int vertexNums;
uniform float time;
uniform int sumFaceVerticesBeforeEndMesh;
uniform sampler2D waveMap;

uniform mat4 lightSpaceMatrix;

const float PI = 3.14159265359;

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

float smoothCurve(float x)
{
	return x * x * (3.0 - 2.0 * x);
}

float trianglewave(float x)
{
	return abs(fract(x +0.5) * 2.0 - 1.0);
}

float smoothTriangleWave(float x)
{
	return smoothCurve(trianglewave(x));
}

void main()
{
	if(planeOrTree < 0)
	{
		gl_Position =lightSpaceMatrix* model * vec4(aPos, 1.0);
	}
	else
	{
		//sum_u[gl_InstanceID*assimpvertexNums+positionIndex]=u[treeFrameIndex[gl_InstanceID][0]*frameNums*vertexNums+treeFrameIndex[gl_InstanceID][1]*vertexNums+faceId]+sum_u[gl_InstanceID*assimpvertexNums+positionIndex];
	
		vec4 tempPos=sum_u[gl_InstanceID*assimpvertexNums+positionIndex]+vec4(0,0,0,1.0f);
        //vec4 tempPos=vec4(aPos,1.0);
		testSumU=tempPos;
		tempPos = model * instanceMatrix * tempPos;
		if(positionIndex >= sumFaceVerticesBeforeEndMesh)
		{
			vec3 wordPos = vec3(model *instanceMatrix* tempPos);
//			float speed = 0.05;
//			float windStrong = 2 * sin(frameIndex);
//			float magnitude = (sin((tempPos.y+tempPos.x+frameIndex*PI/((28.0)*speed)))*0.15+0.15)*0.30;
//			float grassWeight = (pow(aTexCoords.x-0.5,2) + pow(1-aTexCoords.y,2))*2;
//			magnitude *= magnitude * grassWeight;
//			float d0 = sin(frameIndex * PI/(112.0 * speed)) * 3.0-1.5;
//			float d1 = sin(frameIndex * PI/(142.0 * speed)) * 3.0-1.5;
//			float d2 = sin(frameIndex * PI/(132.0 * speed)) * 3.0-1.5;
//			float d3 = sin(frameIndex * PI/(122.0 * speed)) * 3.0-1.5;
//			tempPos.x += sin((frameIndex * PI / (11.0 * speed)) + (tempPos.z + d2) + (tempPos.x + d3)) * (magnitude/2.0) * (1.0f + windStrong * 1.0f);
//			tempPos.y += sin((frameIndex * PI / (17.0 * speed)) + (tempPos.z + d2)*1.6 + (-tempPos.x + d3)*1.6) * magnitude * (1.0f + windStrong * 1.0f);
//			tempPos.z += sin((frameIndex * PI / (11.0 * speed)) + (tempPos.z + d2) + (tempPos.x + d3)) * (magnitude/2.0) * (1.0f + windStrong * 1.0f);
//			gl_Position = tempPos;
			//gl_Position = tempPos + vec4(0.0f,  pow(texture(waveMap, aTexCoords).x,6) * sin(positionIndex * frameIndex) * 0.05f,0.0f,0);
			//gl_Position = tempPos + vec4(0.0f,  pow((1 - (pow(aTexCoords.x-0.5,2) + pow(aTexCoords.y,2))),2) * sin(frameIndex)* 0.05,0.0f,0);
			//gl_Position = tempPos + vec4(0.0f,  pow(texture(waveMap, aTexCoords).x,4) * sin(frameIndex)* 0.06,0.0f,0);
//			vec4 windDirection = vec4(0.0,1.0,0.0,1.0);
//			float phase = 0.05;//频率
//			float flutter = 0.1;//摆动幅度
//			float primaryOffset = 0.008;//所有顶点固定偏移
//			float fDetailAmp = 0.1f;
//			//顶点部分使用世界坐标位置作为随机因子
//			float leafPhase = dot(wordPos,vec3(1.0));
//			float wavesIn = time + leafPhase + phase;
//			float waves = fract(wavesIn * 1.975)*2.0 - 1.0;
//			waves = 0.05 * smoothTriangleWave(waves);
//			vec3 bend = flutter * fDetailAmp * aNormal.xyz;
//			//bend.y = 0.05*0.3;
//			//限制根部附近顶点的运动
//			float animfade = pow((1 - (pow(aTexCoords.x-0.5,2) + pow(aTexCoords.y,2))),2);
//			//最终混合：抖动（平滑过的三角波）+次偏移（整体来回摆动）
//			tempPos.xyz += ((waves * bend) + (windDirection.xyz * waves))*windDirection.w*animfade;
//			tempPos.xyz += primaryOffset * windDirection.xyz *animfade;


			gl_Position =lightSpaceMatrix* tempPos;
		}
		else
		{
			gl_Position = lightSpaceMatrix* tempPos;
		}
	}
	
}
