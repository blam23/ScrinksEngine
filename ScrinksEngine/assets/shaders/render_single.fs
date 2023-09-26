#version 330 core
layout (location = 0) out vec3 gPosition;
layout (location = 1) out vec3 gNormal;
layout (location = 2) out vec4 gAlbedo;

in vec2 texCoord;
in vec3 fragPos;
in vec3 normal;

uniform sampler2D albedoTex;
uniform sampler2D normalTex;
uniform sampler2D depthTex;

void main()
{
    gAlbedo = texture(albedoTex, texCoord);
    gAlbedo.a = 1.0;
    gPosition = fragPos * texture(depthTex, texCoord).r;
    gNormal = normal * texture(albedoTex, texCoord).rgb;
}