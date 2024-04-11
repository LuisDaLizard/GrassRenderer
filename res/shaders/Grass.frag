#version 450

#define MAX_COLORS 256

layout(location = 0) out vec4 oColor;

layout(binding = 2) uniform FragmentUniforms
{
    vec4 uColors[MAX_COLORS];
    bool uShowPatches;
} Uniforms;

layout(location = 0) flat in float tePatch;
layout(location = 1) in float teHeight;

void main()
{
    if (Uniforms.uShowPatches)
    {
        int patchIndex = int(floor(tePatch));
        oColor = Uniforms.uColors[patchIndex];
    }
    else
    {
        oColor = vec4(0, teHeight, 0, 1);
    }
}