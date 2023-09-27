#version 330 core
out vec3 colour;

in vec2 texCoord;

uniform sampler2D gPosition;
uniform sampler2D gNormal;
uniform sampler2D gAlbedoSpec;
uniform sampler2D ssao;

struct Light {
    vec3 Position;
    vec3 Color;
    
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

    vec3 ambientDiffuse = max(dot(worldNormal, ambientLightDir), 0.0) * ambientLightColour;
    vec3 lighting = (albedo) + ((ambientDiffuse*2.0) * ambientLightIntensity);
    lighting -= max(1,-worldPos.y)*testA;

    colour = lighting * ao;
}