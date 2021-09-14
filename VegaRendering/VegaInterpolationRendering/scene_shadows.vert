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
out float v2f_Color;
out vec4 v2f_FragposLightPos;
//out mat4 v2f_instanceMatrix;


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
	v2f_TexCoords = aTexCoords; 
	if(planeOrTree < 0)
	{
		v2f_Normal = mat3(model) * aNormal;
		v2f_WorldPos = vec3(model * vec4(aPos,1.0));
		//v2f_Color=-1.0;
		v2f_FragposLightPos=lightSpaceMatrix*vec4(v2f_WorldPos,1.0);
		v2f_Color=-10.0;


		gl_Position = projection * view * model * vec4(aPos, 1.0);	 
	}
	else
	{
		v2f_Color=1.0;
		vec4 tempPos=vec4(aPos,1.0) + sum_u[gl_InstanceID*assimpvertexNums+positionIndex];
		v2f_Normal = mat3(model)* mat3(instanceMatrix) * aNormal;
		vec3 normal = mat3(model)* mat3(instanceMatrix) * aNormal;
		v2f_WorldPos = vec3(model *instanceMatrix* tempPos);
		v2f_FragposLightPos=lightSpaceMatrix*vec4(v2f_WorldPos,1.0);
	    tempPos = projection * view * model * instanceMatrix * tempPos;
		
		if(positionIndex >= sumFaceVerticesBeforeEndMesh)
		{

			/*************version1******************/
			vec4 windDirection = vec4(1.0,0.0,0.0,1.0);
			
			//////////////////////////��μ���ÿһƬҶ�ӵ����Ĵ�С����������������������������
			float force = sin(0.1* time*v2f_WorldPos.x)+1;
			float phase = 0.05;//Ƶ��
			float flutter = 1.0;//�ڶ�����
			float primaryOffset = 0.08;//���ж���̶�ƫ��
			//���㲿��ʹ����������λ����Ϊ�������
			float leafPhase = dot(v2f_WorldPos,vec3(1.0));
			//float leafPhase = dot(v2f_WorldPos,windDirection.xyz); 
			vec2 wavesIn = vec2(time) + vec2(leafPhase,phase);
			//float wavesIn = leafPhase;
			vec4  waves = (fract(wavesIn.xxyy * vec4(1.975,0.793,0.375,0.193))*2.0 - 1.0);
			waves = 0.08 * smoothTriangleWave(waves);
			vec2 waveSum = waves.xz +waves.yw;
			//waves = sin(0.3*waves);
			vec3 bend = vec3(flutter) * v2f_Normal.xyz;
			//bend.y = 0.05*0.3;
			//���Ƹ�������������˶�
			//gl_Position = tempPos + vec4(0.0f,  pow((1 - (pow(aTexCoords.x-0.5,2) + pow(aTexCoords.y,2))),2) * sin(frameIndex)* 0.05,0.0f,0);
			//���ջ�ϣ�������ƽ���������ǲ���+��ƫ�ƣ��������ذڶ���
			/////(vec3(��*������+ vec3(����*��))*float*float
			//tempPos.xyz +=  ((waveSum.xyx * bend) + bendScale * (windDirection.xyz * waveSum.y))*windDirection.w*animfade;
			//���緽����ƫ��
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

