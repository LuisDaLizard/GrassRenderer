#version 400 core

layout (location = 0) in vec3 aPosition;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in float aPatch;

uniform float uHeight;

flat out float cPatch;
flat out vec4 cBladeDir;
flat out vec4 cUpDir;
flat out vec4 cV2;

void main()
{
    cPatch = aPatch;
    cUpDir = vec4(aNormal, 0);
    cBladeDir = vec4(1, 0, 0, 0);
    cV2 = vec4(aPosition, 1) + uHeight * cUpDir;

    gl_Position = vec4(aPosition, 1);
}