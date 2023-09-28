#version 330 core
layout (location = 0) out vec3 gPosition;
layout (location = 1) out vec3 gNormal;
layout (location = 2) out vec4 gAlbedo;

in vec2 texCoord;
in vec3 fragPos;
in vec3 normal;

uniform sampler2D tex1;
uniform sampler2D tex2;

uniform float testA;

void main()
{
    gAlbedo = texture(tex1, texCoord);
    gPosition = fragPos;
    gNormal = normalize(normal);
}