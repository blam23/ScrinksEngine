#version 330 core
layout (location = 0) in vec4 meshData; // (mesh relative pos, tex coords)
layout (location = 1) in vec3 instanceData; // (instance pos, tileIdx)

out vec2 texCoord;
out float tileIdx;

uniform mat4 projection;
uniform vec2 scale;

void main()
{
    texCoord.x = meshData.z;
    texCoord.y = -meshData.w;
    tileIdx = instanceData.z;
    gl_Position = projection * vec4((meshData.xy * scale) + instanceData.xy, 0.0, 1.0);
}