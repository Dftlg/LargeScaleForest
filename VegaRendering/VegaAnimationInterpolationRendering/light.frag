#version 330 core
out vec4 FragColor;

in vec3 TexColor;

uniform sampler2D texture_diffuse1;

void main()
{    
    //FragColor = texture(texture_diffuse1, TexCoords);
	FragColor = vec4(TexColor,1.0);
}