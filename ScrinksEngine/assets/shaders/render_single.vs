#version 330 core
layout(location = 0) in vec4 data;

out vec2 texCoord;
out vec3 fragPos;
out vec3 normal;

uniform mat4 projection;
uniform mat4 model;

void main()
{
    vec4 worldPos = model * vec4(data.xy, 0.0, 0.5);
    fragPos = worldPos.xyz;
    texCoord = data.wz;

    mat3 normalMatrix = transpose(inverse(mat3(model)));
    normal = normalMatrix * vec3(0.0, 1.0, 0.0);

    gl_Position = projection * worldPos + vec4(normal * 0.1f, 0);
}