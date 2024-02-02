#version 330 core
out vec4 colour;

in vec2 texCoord;
in float tileIdx;

uniform sampler2D tex;
uniform int tiles;

void main()
{
    int tdx = int(tileIdx);
    vec2 offset = vec2(float(tdx % tiles), tdx /tiles);
    vec4 sample = texture(tex, (texCoord+offset)/tiles);
    colour = sample;
}