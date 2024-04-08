#version 450

layout(location = 0) in vec3 aPosition;
layout(location = 1) in vec3 aNormal;

layout(binding = 0) uniform UniformMatrices
{
    mat4 uProj;
    mat4 uView;
    mat4 uWorld;
} Uniforms;

layout(location = 0) out vec3 fNormal;
layout(location = 1) out vec3 fPosition;

void main()
{
    fNormal = aNormal;
    fPosition = (Uniforms.uWorld * vec4(aPosition, 1)).xyz;
    gl_Position = Uniforms.uProj * Uniforms.uView * Uniforms.uWorld * vec4(aPosition, 1);
}