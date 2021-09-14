#version 430 core
out vec4 FragColor;

in vec2 v2f_TexCoords;
in vec3 v2f_WorldPos;
in vec3 v2f_Normal;

// material parameters
//uniform vec3 albedo;
uniform float metallic;
uniform float roughness;
uniform float ao;

// lights
uniform vec3 lightPositions[4];
uniform vec3 lightColors[4];

uniform vec3 camPos;
//uniform vec3 viewPos;

uniform float far_plane;
uniform bool shadows;

uniform sampler2D texture_diffuse1;
uniform sampler2D texture_opacity1;
uniform samplerCube depthMap;

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

// ----------------------------------------------------------------------------
float ShadowCalculation(vec3 fragPos,vec3 lightPos)
{
    vec3 fragToLight = fragPos - lightPos;
    float currentDepth = length(fragToLight);
	float shadow = 0.0;
    float bias = 0.05;
    int samples = 65;
    float viewDistance = length(camPos - fragPos);
    float diskRadius = (1.0 + (viewDistance / far_plane)) / 25.0;
    for(int i = 0; i < samples; ++i)
    {
        float closestDepth = texture(depthMap, fragToLight + gridSamplingDisk[i] * diskRadius).r;
        closestDepth *= far_plane;   // undo mapping [0;1]
        if(currentDepth - bias > closestDepth)
            shadow += 1.0;
    }
    shadow /= float(samples);
	
	//FragColor = vec4(vec3(closestDepth / far_plane), 1.0);       
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
	if(texColor2.rgb == vec3(0.0,0.0,0.0))
		discard;	
    //FragColor = texColor1;

	vec3 albedo = pow(texture(texture_diffuse1, v2f_TexCoords).rgb, vec3(2.2));

    vec3 N = normalize(v2f_Normal);
    vec3 V = normalize(camPos - v2f_WorldPos);

    vec3 F0 = vec3(0.04); 
    F0 = mix(F0, albedo, metallic);

    vec3 Lo = vec3(0.0);
	float shadow = 0.0;
	 for(int i = 0; i < 4; ++i) 
    {
        vec3 L = normalize(lightPositions[i] - v2f_WorldPos);
        vec3 H = normalize(V + L);
        float distance = length(lightPositions[i] - v2f_WorldPos);
        float attenuation = 1.0 / (distance * distance);
        //vec3 radiance = lightColors[i] * attenuation;
		vec3 radiance = lightColors[i];

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
    }   
    shadow = shadows ? ShadowCalculation(v2f_WorldPos,lightPositions[0]) : 0.0;

    vec3 ambient = vec3(0.03) * albedo * ao;

	vec3 color = Lo * (1.0 - shadow) + ambient;

    // HDR tonemapping
    color = color / (color + vec3(1.0));
    // gamma correct
    color = pow(color, vec3(1.0/2.2)); 

    FragColor = vec4(color, 1.0);

	
}

