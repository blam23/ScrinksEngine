#version 330 core
out vec3 colour;

in vec2 texCoord;

uniform sampler2D gPosition;
uniform sampler2D gNormal;
uniform sampler2D gAlbedoSpec;
uniform sampler2D gDepth;
uniform sampler2D ssao;
uniform sampler2D shadowMap;

uniform samplerCube skyMap;

uniform mat4 lightSpaceMatrix;
uniform mat4 inverseView;

uniform float metallic;
uniform float roughness;

uniform vec3 lightPositions[4];
uniform vec3 lightColors[4];
uniform vec3 camPos;

const float PI = 3.14159265359;

vec3 fresnelSchlick(float cosTheta, vec3 F0)
{
    return F0 + (1.0 - F0) * pow(clamp(1.0 - cosTheta, 0.0, 1.0), 5.0);
}

vec3 fresnelSchlickRoughness(float cosTheta, vec3 F0, float roughness)
{
    return F0 + (max(vec3(1.0 - roughness), F0) - F0) * pow(clamp(1.0 - cosTheta, 0.0, 1.0), 5.0);
}   

float DistributionGGX(vec3 N, vec3 H, float roughness)
{
    float a      = roughness*roughness;
    float a2     = a*a;
    float NdotH  = max(dot(N, H), 0.0);
    float NdotH2 = NdotH*NdotH;
	
    float num   = a2;
    float denom = (NdotH2 * (a2 - 1.0) + 1.0);
    denom = PI * denom * denom;
	
    return num / denom;
}

float GeometrySchlickGGX(float NdotV, float roughness)
{
    float r = (roughness + 1.0);
    float k = (r*r) / 8.0;

    float num   = NdotV;
    float denom = NdotV * (1.0 - k) + k;
	
    return num / denom;
}
float GeometrySmith(vec3 N, vec3 V, vec3 L, float roughness)
{
    float NdotV = max(dot(N, V), 0.0);
    float NdotL = max(dot(N, L), 0.0);
    float ggx2  = GeometrySchlickGGX(NdotV, roughness);
    float ggx1  = GeometrySchlickGGX(NdotL, roughness);
	
    return ggx1 * ggx2;
}

float calc_shadow(vec3 pos, vec3 normal)
{
    vec4 fragPosLightSpace = lightSpaceMatrix * vec4(pos, 1.0);

    vec3 projCoords = fragPosLightSpace.xyz / fragPosLightSpace.w;
    projCoords = projCoords * 0.5 + 0.5;
    float closestDepth = texture(shadowMap, projCoords.xy).r;
    float currentDepth = projCoords.z;

    float bias = 0.005;
    float shadow = 0.0;

    vec2 texelSize = 1.0 / textureSize(shadowMap, 0);
    for(int x = -1; x <= 1; ++x)
    {
        for(int y = -1; y <= 1; ++y)
        {
            float pcfDepth = texture(shadowMap, projCoords.xy + vec2(x, y) * texelSize).r; 
            shadow += currentDepth - bias > pcfDepth ? 1.0 : 0.0;
        }
    }
    shadow /= 9.0;

    if(projCoords.z > 1.0)
        shadow = 0.0;
    
    return shadow;
}


void main()
{
    vec4  sample      = texture(gAlbedoSpec, texCoord);
    vec3  albedo      = sample.rgb;
    vec3  worldNormal = texture(gNormal, texCoord).rgb;
    float ao          = texture(ssao, texCoord).r;
    vec3  viewPos     = (texture(gPosition, texCoord)).xyz;
    vec3  worldPos    = (inverseView * vec4(viewPos, 1.0)).xyz;
    float depth       = texture(gDepth, texCoord).r;
    float shadow      = calc_shadow(worldPos, worldNormal);

    vec3 N = normalize(worldNormal);
    vec3 V = normalize(camPos - worldPos);
    vec3 R = reflect(-V, N);

    vec3 F0 = vec3(0.04); 
    F0 = mix(F0, albedo, metallic);

    vec3 Lo = vec3(0);
    for (int i = 0; i < 4; ++i)
    {
        vec3 L = normalize(lightPositions[i] - worldPos);
        vec3 H = normalize(V + L);

        float dist = length(lightPositions[i] - worldPos);
        float attenuation = 1.0 / (dist * dist);
        vec3 radiance = lightColors[i] * attenuation;

        vec3 F0 = vec3(0.04);
        F0      = mix(F0, albedo, metallic);
        vec3 F  = fresnelSchlick(max(dot(H, V), 0.0), F0);

        float NDF = DistributionGGX(N, H, roughness);
        float G   = GeometrySmith(N, V, L, roughness);

        vec3 numerator    = NDF * G * F;
        float denominator = 4.0 * max(dot(N, V), 0.0) * max(dot(N, L), 0.0)  + 0.0001;
        vec3 specular     = numerator / denominator;

        vec3 kS = F;
        vec3 kD = vec3(1.0) - kS;
        
        kD *= 1.0 - metallic;

  
        float NdotL = max(dot(N, L), 0.0);
        Lo += (kD * albedo / PI + specular) * radiance * NdotL;
    }

    vec3 kS = fresnelSchlickRoughness(max(dot(N, V), 0.0), F0, roughness);
    vec3 kD = 1.0 - kS;
    vec3 irradiance = vec3(0.8,1,1);//texture(skyMap, N).rgb;
    vec3 diffuse    = irradiance * albedo;
    vec3 ambient    = (kD * diffuse) * ao;
    colour = ambient + Lo - (shadow * 0.1);

    if (depth > 0.999)
        colour = texture(skyMap, vec3(0.5,0.5,1)).rgb;
}