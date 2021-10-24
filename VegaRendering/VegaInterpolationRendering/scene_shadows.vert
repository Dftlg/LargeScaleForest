#version 430 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec2 aTexCoords;
layout (location = 3) in int faceId;
layout (location = 4) in mat4 instanceMatrix;
layout (location = 8 ) in int positionIndex;

out vec2 v2g_TexCoords;
out vec3 v2g_WorldPos;;
out vec3 v2g_Normal;
out float v2g_Color;
out vec4 v2g_FragposLightPos;
//out mat4 v2f_instanceMatrix;

//out vec4 TEST_BASEPosition;
out mat4 TEST_BASEMat;

uniform int frameNums;
uniform int frameIndex;
uniform int vertexNums;
uniform int assimpvertexNums;
uniform mat4 projection;
uniform mat4 view;
uniform mat4 model;
uniform int planeOrTree;
uniform float time;
uniform float windStrength;
uniform int sumFaceVerticesBeforeEndMesh;
uniform float bendScale;
uniform float primaryOffsetScale;

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

layout (std430, binding=4) buffer bufferNormal
{
	vec4 Comp_nurmal[];
};

vec4 smoothCurve(vec4 x)
{
	return x * x * (3.0 - 2.0 * x);
}

vec4 trianglewave(vec4 x)
{
	return abs(fract(x +0.5) * 2.0 - 1.0);
}

vec4 smoothTriangleWave(vec4 x)
{
	return smoothCurve(trianglewave(x));
}

void main()
{
	
	if(planeOrTree < 0)
	{

		v2g_Normal = mat3(model) * aNormal;
		v2g_WorldPos = vec3(model * vec4(aPos,1.0));
		//v2f_Color=-1.0;
		v2g_FragposLightPos=lightSpaceMatrix*vec4(v2g_WorldPos,1.0);
		v2g_Color=-10.0;
		gl_Position = projection * view * model * vec4(aPos, 1.0);	 
	}
	else
	{
		
		//vec4 tempPos=vec4(aPos,1.0) + sum_u[gl_InstanceID*assimpvertexNums+positionIndex];
		vec4 tempPos=sum_u[gl_InstanceID*assimpvertexNums+positionIndex]+vec4(0,0,0,1.0f);
        //vec4 tempPos=vec4(aPos,1.0);
		//v2g_Normal = mat3(model)* mat3(instanceMatrix) * aNormal;
		v2g_Normal = mat3(model)* mat3(instanceMatrix) * aNormal;
		v2g_Color=1.0;
		v2g_WorldPos = vec3(model *instanceMatrix* tempPos);
		v2g_FragposLightPos=lightSpaceMatrix*vec4(v2g_WorldPos,1.0);
		v2g_TexCoords = aTexCoords; 
		
		//TEST_BASEPosition=tempPos;
		TEST_BASEMat=projection * view * model * instanceMatrix;
		//vec3 normal = mat3(model)* mat3(instanceMatrix) * aNormal;
	    tempPos = projection * view * model * instanceMatrix * tempPos;

		
		
		if(positionIndex >= sumFaceVerticesBeforeEndMesh)
		{

			/*************version1******************/
			vec4 windDirection = vec4(1.0,0.0,0.0,1.0);
			
			//////////////////////////如何计算每一片叶子的力的大小？？？？？？？？？？？？？？
			float force = sin(0.1* time*v2g_WorldPos.x)+1;
			float phase = 0.05;//频率
			float flutter = 1.0;//摆动幅度
			float primaryOffset = 0.08;//所有顶点固定偏移
			//顶点部分使用世界坐标位置作为随机因子
			float leafPhase = dot(v2g_WorldPos,vec3(1.0));
			//float leafPhase = dot(v2f_WorldPos,windDirection.xyz); 
			vec2 wavesIn = vec2(time) + vec2(leafPhase,phase);
			//float wavesIn = leafPhase;
			vec4  waves = (fract(wavesIn.xxyy * vec4(1.975,0.793,0.375,0.193))*2.0 - 1.0);
			waves = 0.08 * smoothTriangleWave(waves);
			vec2 waveSum = waves.xz +waves.yw;
			//waves = sin(0.3*waves);
			vec3 bend = vec3(flutter) * v2g_Normal.xyz;
			//bend.y = 0.05*0.3;
			//限制根部附近顶点的运动
			//gl_Position = tempPos + vec4(0.0f,  pow((1 - (pow(aTexCoords.x-0.5,2) + pow(aTexCoords.y,2))),2) * sin(frameIndex)* 0.05,0.0f,0);
			//最终混合：抖动（平滑过的三角波）+次偏移（整体来回摆动）
			/////(vec3(波*弯曲）+ vec3(风向*波))*float*float
			//tempPos.xyz +=  ((waveSum.xyx * bend) + bendScale * (windDirection.xyz * waveSum.y))*windDirection.w*animfade;
			//往风方向主偏移
			//float primaryOffset * vec3 windDirection * float animfade * float force
			//tempPos.xyz += primaryOffsetScale * primaryOffset * windDirection.xyz * animfade * force;


			gl_Position = tempPos;
		}
		else
		{
			gl_Position =  tempPos;
		}
	
		
	}
}

