#version 330 core
out vec3 colour;

in vec2 texCoord;

uniform sampler2D gPosition;
uniform sampler2D gNormal;
uniform sampler2D gAlbedoSpec;
uniform sampler2D gDepth;
uniform sampler2D ssao;
uniform sampler2D shadowMap;

struct Light {
    vec3 Position;
    vec3 Colour;
    
    float Linear;
    float Quadratic;
};
const int NR_LIGHTS = 32;
uniform Light lights[NR_LIGHTS];

const vec3 ambientLightDir = vec3(0.5, -1.0, 1.0);
uniform vec3 ambientLightColour;
uniform float ambientLightIntensity;
uniform mat4 inverseView;
uniform mat4 lightSpaceMatrix;

uniform float testA;
uniform float testB;
uniform float testC;
uniform float testD;

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
    float sampleSpec  = sample.a;
    float depth       = texture(gDepth, texCoord).r;
    vec3  worldNormal = texture(gNormal, texCoord).rgb;
    float ao          = texture(ssao, texCoord).r;
    vec3  viewPos     = (texture(gPosition, texCoord)).xyz;
    vec3  worldPos    = (inverseView * vec4(viewPos, 1.0)).xyz;
    float shadow      = calc_shadow(worldPos, worldNormal);

    // blinn-phong (in view-space)
    vec3 ambient = vec3(0.3 * albedo * ao) - (shadow * 0.15); // here we add occlusion factor
    vec3 lighting  = ambient;
    
    //vec3 viewDir  = normalize(-worldPos); // viewpos is (0.0.0) in view-space
    // diffuse
    // for(int i = 0; i < NR_LIGHTS; i++)
    // {
    //     vec3 lightDir = normalize(lights[i].Position - worldPos);
    //     vec3 diffuse = max(dot(worldNormal, lightDir), 0.0) * albedo * lights[i].Colour;
    //     // specular
    //     vec3 halfwayDir = normalize(lightDir + viewDir); 
    //     float spec = pow(max(dot(worldNormal, halfwayDir), 0.0), 8.0);
    //     vec3 specular = lights[i].Colour * spec;
    //     // attenuation
    //     float dist = length(lights[i].Position - worldPos);
    //     float attenuation = 1.0 / (1.0 + lights[i].Linear * dist + lights[i].Quadratic * dist * dist);
    //     diffuse  *= attenuation;
    //     specular *= attenuation;
    //     lighting += diffuse + specular;
    // }

    vec3 ambientDiffuse = max(dot(worldNormal, normalize(ambientLightDir)), 0.0) * vec3(1, 1, 1);
    lighting += ((ambientDiffuse*2.0) * ambientLightIntensity);

    colour = vec3(0.1, 0.1, 0.1) + (ao - (shadow * 0.2)) * 0.5;

    //colour = lighting * pow(albedo, vec3(2.0));

    colour += vec3((worldPos.y - 10) / 30);
    colour *= lighting * albedo;

    if (depth > 0.999)
       colour = vec3(0.1, 0.1, 0.3);
}