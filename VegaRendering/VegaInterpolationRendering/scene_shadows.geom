#version 430 core
layout (triangles) in;
layout (triangle_strip, max_vertices = 3) out;

in vec2 v2g_TexCoords[];
in vec3 v2g_WorldPos[];
in vec3 v2g_Normal[];
in float v2g_Color[];
in vec4 v2g_FragposLightPos[];

out vec2 v2f_TexCoords;
out vec3 v2f_WorldPos;
out vec3 v2f_Normal;
out float v2f_Color;
out vec4 v2f_FragposLightPos;

const float MAGNITUDE = 0.9;
uniform float time;
uniform int sumFaceVerticesBeforeEndMesh;
uniform int planeOrTree;
uniform mat4 model;
uniform mat4 projection;
uniform mat4 view;

vec3 GetNormal() 
{
    vec3 a = vec3(gl_in[0].gl_Position) - vec3(gl_in[1].gl_Position);
    vec3 b = vec3(gl_in[2].gl_Position) - vec3(gl_in[1].gl_Position);
    return normalize(cross(a, b));
}

void main()
{
   
	if(planeOrTree > 0)
	{
		vec3 normal = GetNormal();
		for( int i=0; i<gl_in.length( ); ++i )
         {
            gl_Position = gl_in[i].gl_Position;
            v2f_TexCoords=v2g_TexCoords[i];
            v2f_WorldPos=v2g_WorldPos[i];
            v2f_Color=v2g_Color[i];
            v2f_FragposLightPos=v2g_FragposLightPos[i];
            //v2f_Normal=v2g_Normal[i];
            //g2f_Normal = normal;
			v2f_Normal=normal;
            EmitVertex( );
         }

    EndPrimitive( );
	}
    else
    {
        //g2f_Normal=v2g_Normal[0];
    }
    
}
