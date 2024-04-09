#version 450

layout (location = 0) in vec3 aPosition;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec3 aDirection;
layout (location = 3) in float aPatch;

layout(location = 0) flat out float vPatch;
layout(location = 1) flat out vec3 vV1;
layout(location = 2) flat out vec3 vV2;
layout(location = 3) flat out vec3 vBladeDir;
layout(location = 4) flat out vec3 vBladeUp;

void main()
{
    float height = 0.5f;

    vPatch = aPatch - 0.01;
    vBladeUp = aNormal;
    vBladeDir = aDirection;
    vV1 = aPosition + (height / 2.0) * vBladeUp;
    vV2 = aPosition + height * vBladeUp;

    gl_Position = vec4(aPosition, 1);
}