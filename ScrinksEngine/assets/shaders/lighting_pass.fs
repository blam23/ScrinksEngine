#version 330 core
out vec3 colour;

in vec2 texCoord;

uniform sampler2D gPosition;
uniform sampler2D gNormal;
uniform sampler2D gAlbedoSpec;
uniform sampler2D ssao;

struct Light {
    vec3 Position;
    vec3 Colour;
    
    float Linear;
    float Quadratic;
};
const int NR_LIGHTS = 32;
uniform Light lights[NR_LIGHTS];
uniform vec3 viewPos;

uniform vec3 ambientLightDir;
uniform vec3 ambientLightColour;
uniform float ambientLightIntensity;

uniform float testA;
uniform float testB;
uniform float testC;

uniform mat4 view;

void main()
{
    vec4  sample      = texture(gAlbedoSpec, texCoord);
    vec3  albedo      = sample.rgb;
    float sampleSpec  = sample.a;
    vec4  fragPos     = texture(gPosition, texCoord);
    vec3  worldPos    = (fragPos * view).rgb;
    vec3  viewNormal  = texture(gNormal, texCoord).rgb;
    vec3  worldNormal = viewNormal * transpose(inverse(mat3(view)));
    float ao          = texture(ssao, texCoord).r;


    // blinn-phong (in view-space)
    vec3 ambient = vec3(0.3 * albedo * ao); // here we add occlusion factor
    vec3 lighting  = ambient; 
    vec3 viewDir  = normalize(-worldPos); // viewpos is (0.0.0) in view-space
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

    vec3 ambientDiffuse = max(dot(worldNormal, normalize(ambientLightDir)), 0.0) * ambientLightColour;
    lighting += ((ambientDiffuse*2.0) * ambientLightIntensity);

    colour = lighting;
}