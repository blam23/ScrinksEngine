#version 330 core
layout (location = 0) out vec3 gPosition;
layout (location = 1) out vec3 gNormal;
layout (location = 2) out vec3 gAlbedo;
layout (location = 3) out vec3 gMRAO;

in vec3 fragPos;

void main()
{
    // Just update the sky position, lighting can figure out the rest
    gPosition = vec3(0);
}