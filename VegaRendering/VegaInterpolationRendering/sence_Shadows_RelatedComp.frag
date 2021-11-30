#version 430 core
out vec4 FragColor;

in vec2 v2f_TexCoords;
in vec3 v2f_WorldPos;
in vec3 v2f_Normal;
in float v2f_Color;
in vec4 v2f_FragposLightPos;

in vec3 g2f_Normal;

// material parameters
//uniform vec3 albedo;
uniform float metallic;
uniform float roughness;
uniform float ao;

// lights
uniform vec3 lightPositions;
uniform vec3 lightColors;

uniform vec3 camPos;
uniform vec3 viewPos;

uniform float far_plane;
uniform bool shadows;

uniform sampler2D texture_diffuse1;
uniform sampler2D texture_opacity1;
uniform sampler2D depthMap;


const float PI = 3.14159265359;

// array of offset direction for sampling
vec3 gridSamplingDisk[20] = vec3[]
(
   vec3(1, 1,  1), vec3( 1, -1,  1), vec3(-1, -1,  1), vec3(-1, 1,  1), 
   vec3(1, 1, -1), vec3( 1, -1, -1), vec3(-1, -1, -1), vec3(-1, 1, -1),
   vec3(1, 1,  0), vec3( 1, -1,  0), vec3(-1, -1,  0), vec3(-1, 1,  0),
   vec3(1, 0,  1), vec3(-1,  0,  1), vec3( 1,  0, -1), vec3(-1, 0, -1),
   vec3(0, 1,  1), vec3( 0, -1,  1), vec3( 0, -1, -1), vec3( 0, 1, -1)
);

float ShadowCalculation(vec4 fragPos)
{
	float shadow = 0.0;
    vec3 fragToLight = fragPos.xyz/fragPos.w;
	fragToLight=fragToLight*0.5+0.5;
    float currentDepth = fragToLight.z;

	//vec3 normal = normalize(v2f_Normal);
	vec3 normal;

	normal=normalize(v2f_Normal);

    vec3 lightDir = normalize(lightPositions - v2f_WorldPos);
    float bias = max(0.05 * (1.0 - dot(normal, lightDir)), 0.005);

	//float shadow = 0.0;
    vec2 texelSize = 1.0 / textureSize(depthMap, 0);
    for(int x = -1; x <= 1; ++x)
    {
        for(int y = -1; y <= 1; ++y)
        {
            float pcfDepth = texture(depthMap, fragToLight.xy + vec2(x, y) * texelSize).r; 
            shadow += currentDepth - bias > pcfDepth  ? 1.0 : 0.0;        
        }    
    }
    shadow /= 9.0;
    
    // keep the shadow at 0.0 when outside the far_plane region of the light's frustum.
    if(fragToLight.z > 1.0)
        shadow = 0.0;
    return shadow;
}

// ----------------------------------------------------------------------------
float DistributionGGX(vec3 N, vec3 H, float roughness)
{
    float a = roughness*roughness;
    float a2 = a*a;
    float NdotH = max(dot(N, H), 0.0);
    float NdotH2 = NdotH*NdotH;

    float nom   = a2;
    float denom = (NdotH2 * (a2 - 1.0) + 1.0);
    denom = PI * denom * denom;

    return nom / max(denom, 0.001); // prevent divide by zero for roughness=0.0 and NdotH=1.0
}
// ----------------------------------------------------------------------------
float GeometrySchlickGGX(float NdotV, float roughness)
{
    float r = (roughness + 1.0);
    float k = (r*r) / 8.0;

    float nom   = NdotV;
    float denom = NdotV * (1.0 - k) + k;

    return nom / denom;
}
// ----------------------------------------------------------------------------
float GeometrySmith(vec3 N, vec3 V, vec3 L, float roughness)
{
    float NdotV = max(dot(N, V), 0.0);
    float NdotL = max(dot(N, L), 0.0);
    float ggx2 = GeometrySchlickGGX(NdotV, roughness);
    float ggx1 = GeometrySchlickGGX(NdotL, roughness);

    return ggx1 * ggx2;
}
// ----------------------------------------------------------------------------
vec3 fresnelSchlick(float cosTheta, vec3 F0)
{
    return F0 + (1.0 - F0) * pow(1.0 - cosTheta, 5.0);
}

void main()
{    
	vec4  texColor1 = texture(texture_diffuse1, v2f_TexCoords);
	vec4  texColor2 = texture(texture_opacity1, v2f_TexCoords);
	if(v2f_Color>0)
	{
		if(texColor2.r < 0.07 && texColor2.g < 0.07 && texColor2.b < 0.07)
			discard;	
    //FragColor = texColor1;
	}
	
	vec3 albedo = pow(texture(texture_diffuse1, v2f_TexCoords).rgb, vec3(2.2));
	

    //vec3 N = normalize(v2f_Normal);
	vec3 N;
    N=normalize(v2f_Normal);

    vec3 V = normalize(camPos - v2f_WorldPos);

    vec3 F0 = vec3(0.04); 
    F0 = mix(F0, albedo, metallic);

    vec3 Lo = vec3(0.0);
	float shadow = 0.0;
	    vec3 L = normalize(lightPositions - v2f_WorldPos);
        vec3 H = normalize(V + L);
        float distance = length(lightPositions - v2f_WorldPos);
        float attenuation = 1.0 / (distance * distance);
        //vec3 radiance = lightColors[i] * attenuation;
		/////////enenene 有问题
		//vec3 radiance = lightColors*1.5;
		vec3 radiance = lightColors;
		//vec3 radiance = lightColors*3;
        // Cook-Torrance BRDF
        float NDF = DistributionGGX(N, H, roughness);   
        float G   = GeometrySmith(N, V, L, roughness);      
        vec3 F    = fresnelSchlick(clamp(dot(H, V), 0.0, 1.0), F0);         
        vec3 nominator    = NDF * G * F; 
        float denominator = 4 * max(dot(N, V), 0.0) * max(dot(N, L), 0.0);
        vec3 specular = nominator / max(denominator, 0.001);    
        vec3 kS = F;
        vec3 kD = vec3(1.0) - kS;
        kD *= 1.0 - metallic;	  
        float NdotL = max(dot(N, L), 0.0);        
        Lo += (kD * albedo / PI + specular) * radiance * NdotL;  		

    shadow = shadows ? ShadowCalculation(v2f_FragposLightPos) : 0.0;

	//FragColor = vec4(shadow,1.0,1.0, 1.0);
    //shadow=1;
    vec3 ambient = vec3(0.03) * albedo * ao;
	vec3 color;
//	if(v2f_Color<0.0)
//	{
//		
////		if(shadow>0.2)
////		{
////			color = Lo * (1.0 - shadow) + ambient;
////		 // HDR tonemapping
////			color = color / (color + vec3(1.0));
////
////			//color=vec3(1.0 - shadow);
////			
////		}
////		else
////		{
////			color=vec3(1.0);
////		}
//		color=vec3(1.0);
//	}
//	else
//	{
		//* (1.0 - shadow)
		Lo*=3;
		color = Lo * (1.0 - shadow) + ambient;
        //color = Lo * (1.0) + ambient;
		 // HDR tonemapping
		color = color / (color + vec3(1.0));
		// gamma correct
		 color = pow(color, vec3(1.0/2.2)); 
	//}
//	if(shadow>0)
// 	 color=vec3(1.0,0.0,0.0);
	//color=vec3(shadow,0.0,0.0);
    //FragColor = vec4(v2f_Normal, 1.0);
	//FragColor = vec4(1.0,1.0,1.0, 1.0);
    FragColor = vec4(color, 1.0);
    //FragColor = vec4(0,0.4,0.4, 1.0);
}
