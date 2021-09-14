#version 430 core
layout (triangles) in;
layout (triangle_strip, max_vertices = 3) out;

in vec2 v2f_texCoords[];
in vec3 v2f_worldPos[];
in vec3 v2f_normal[];
in mat4 v2f_instanceMatrix[];

out vec2 v2f_TexCoords;
out vec3 v2f_WorldPos;
out vec3 v2f_Normal;

const float MAGNITUDE = 0.9;
uniform float time;
uniform int sumFaceVerticesBeforeEndMesh;
uniform int planeOrTree;
uniform mat4 model;
uniform mat4 projection;
uniform mat4 view;

vec3 GetNormal(vec3 position_0,vec3 position_1,vec3 position_2) 
{
	vec3 a = position_0 - position_1;
	vec3 b = position_2 - position_1;
	return normalize(cross(a, b));
}



void main()
{
	if(planeOrTree < 0)
	{

		vec3 position_0 = vec3(gl_in[0].gl_Position);
		vec3 position_1 = vec3(gl_in[1].gl_Position);
		vec3 position_2 = vec3(gl_in[2].gl_Position);
		//vec3 normal = GetNormal(position_0,position_1,position_2);
		gl_Position = gl_in[0].gl_Position;
		v2f_TexCoords = v2f_texCoords[0];
		v2f_WorldPos = v2f_worldPos[0];
		v2f_Normal = v2f_normal[0];
		EmitVertex();
		gl_Position = gl_in[1].gl_Position;
		v2f_TexCoords = v2f_texCoords[1];
		v2f_WorldPos = v2f_worldPos[1];
		v2f_Normal = v2f_normal[1];
		EmitVertex();
		gl_Position = gl_in[2].gl_Position;
		v2f_TexCoords = v2f_texCoords[2];
		v2f_WorldPos = v2f_worldPos[2];
		v2f_Normal = v2f_normal[2];
		EmitVertex();
		EndPrimitive();
	}
	else
	{
		vec3 position_0 = vec3(gl_in[0].gl_Position);
		vec3 position_1 = vec3(gl_in[1].gl_Position);
		vec3 position_2 = vec3(gl_in[2].gl_Position);
		//vec3 normal = GetNormal(position_0,position_1,position_2);
		gl_Position = gl_in[0].gl_Position;
		v2f_TexCoords = v2f_texCoords[0];
		v2f_WorldPos = v2f_worldPos[0];
		v2f_Normal = v2f_normal[0];
		EmitVertex();
		gl_Position = gl_in[1].gl_Position;
		v2f_TexCoords = v2f_texCoords[1];
		v2f_WorldPos =v2f_worldPos[1];
		v2f_Normal = v2f_normal[1];
		EmitVertex();
		gl_Position = gl_in[2].gl_Position;
		v2f_TexCoords = v2f_texCoords[2];
		v2f_WorldPos = v2f_worldPos[2];
		v2f_Normal = v2f_normal[2];
		EmitVertex();
		EndPrimitive();
	}
	
}