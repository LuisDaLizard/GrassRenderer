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
layout(location = 5) flat out float vWidth;

layout(binding = 0) uniform VertexUniforms
{
    float uWidth;
    float uHeight;
} Uniforms;

void main()
{
    vPatch = aPatch - 0.01;
    vBladeUp = aNormal;
    vBladeDir = aDirection;
    vV1 = aPosition + (Uniforms.uHeight / 2.0) * vBladeUp;
    vV2 = aPosition + Uniforms.uHeight * vBladeUp;
    vWidth = Uniforms.uWidth;

    gl_Position = vec4(aPosition, 1);
}