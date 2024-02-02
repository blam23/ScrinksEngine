#version 330 core
layout (location = 0) out vec3 gPosition;
layout (location = 1) out vec3 gNormal;
layout (location = 2) out vec3 gAlbedo;
layout (location = 3) out vec3 gMRAO;

in vec2 texCoord;
in vec3 fragPos;
in vec3 normal;

uniform sampler2D albedo;
uniform sampler2D metallic;
uniform sampler2D roughness;
uniform sampler2D ao;
uniform vec3 tint;

uniform float testA;

void main()
{
    gAlbedo = tint * texture(albedo, texCoord).rgb;
    
    gMRAO.r = texture(metallic, texCoord).r;
    gMRAO.g = texture(roughness, texCoord).r;
    gMRAO.b = texture(ao, texCoord).r;

    gPosition = fragPos;
    gNormal = normalize(normal);
}