#version 400 core

layout (location = 0) in vec3 aPosition;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in float aPatch;

uniform float uHeight;

out float gPatch;
out vec3 gNormal;

void main()
{
    gPatch = aPatch;
    gNormal = aPosition + uHeight * aNormal;
    gl_Position = vec4(aPosition, 1);
}