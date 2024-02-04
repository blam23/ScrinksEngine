#version 330 core
out vec4 colour;

in vec2 texCoord;
in float tileIdx;
in vec2 pixelPos;

uniform sampler2D tex;
uniform int tiles;

void main()
{
    int tdx = int(tileIdx);
    vec2 offset = vec2(float(tdx % tiles), tdx /tiles);
    vec4 sample = texture(tex, (texCoord+offset)/tiles);
    //colour = vec4(pixelPos.x/1920,pixelPos.y/1080, 0.5, round(sample.a));
    //sample.a = sample.a * 1.0-(pixelPos.y / 1080);
    colour = sample;
}