#version 330 core
layout(location = 0) in vec3 iPosition;
layout(location = 1) in vec3 iNormal;
layout(location = 2) in vec2 iTexCoord;

out vec2 texCoord;
out vec3 fragPos;
out vec3 normal;

uniform mat4 projection;
uniform mat4 model;
uniform mat4 view;

void main()
{
    fragPos = (view * model * vec4(iPosition, 1)).xyz;
    texCoord = iTexCoord;

    mat3 normalMatrix = transpose(inverse(mat3(model * view)));
    normal = normalMatrix * iNormal;

    gl_Position = projection * vec4(fragPos, 1);
}