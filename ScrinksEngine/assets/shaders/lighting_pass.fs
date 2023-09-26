#version 330 core
out vec3 colour;

in vec2 texCoord;

uniform sampler2D gPosition;
uniform sampler2D gNormal;
uniform sampler2D gAlbedoSpec;

struct Light {
    vec3 Position;
    vec3 Color;
    
    float Linear;
    float Quadratic;
};
const int NR_LIGHTS = 32;
uniform Light lights[NR_LIGHTS];
uniform vec3 viewPos;

void main()
{
    vec4  sample      = texture(gAlbedoSpec, texCoord);
    vec3  albedo      = sample.rgb;
    float sampleSpec  = sample.a;
    vec3  fragPos     = texture(gPosition, texCoord).rgb;
    vec3  normal      = texture(gNormal, texCoord).rgb;

    // then calculate lighting as usual
    vec3 lighting  = vec3(0.1); // hard-coded ambient component
    vec3 viewDir  = normalize(viewPos - fragPos);
    for(int i = 0; i < 32; ++i)
    {
        // diffuse
        vec3 lightDir = normalize(lights[i].Position - fragPos);
        vec3 diffuse = max(dot(normal, lightDir), 0.0) * lights[i].Color;
        // specular
        vec3 halfwayDir = normalize(lightDir + viewDir);
        float spec = pow(max(dot(normal, halfwayDir), 0.0), 16.0);
        vec3 specular = lights[i].Color * spec * sampleSpec;
        // attenuation
        float distance = length(lights[i].Position - fragPos) * 0.4;
        float attenuation = 1.0 / (1.0 + lights[i].Linear * distance + lights[i].Quadratic * distance * distance);
        diffuse *= attenuation;
        specular *= attenuation;
        lighting += diffuse + specular;
    }
    colour = lighting * albedo;
}