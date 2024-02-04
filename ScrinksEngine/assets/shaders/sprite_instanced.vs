#version 330 core
layout (location = 0) in vec4 meshData; // (mesh relative pos, tex coords)
layout (location = 1) in vec3 instanceData; // (instance pos, tileIdx)

out vec2 texCoord;
out float tileIdx;
out vec2 pixelPos;

uniform mat4 projection;
uniform vec2 scale;

void main()
{
    texCoord = meshData.zw;
    tileIdx  = instanceData.z;
    pixelPos = instanceData.xy;

    gl_Position = projection * vec4((meshData.xy * scale) + pixelPos, 0.0, 1.0);
}