#version 400 core

layout (location = 0) in vec3 aPosition;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec3 aDirection;
layout (location = 3) in float aPatch;

uniform float uHeight;

flat out float vPatch;
flat out vec3 vV1;
flat out vec3 vV2;
flat out vec3 vBladeDir;
flat out vec3 vBladeUp;

void main()
{
    vPatch = aPatch - 0.01;
    vBladeUp = aNormal;
    vBladeDir = aDirection;
    vV1 = aPosition + (uHeight / 2.0) * vBladeUp;
    vV2 = aPosition + uHeight * vBladeUp;

    gl_Position = vec4(aPosition, 1);
}