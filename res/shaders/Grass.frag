#version 400 core

#define MAX_COLORS 256

out vec4 oColor;

layout (std140) uniform Colors
{
    vec4 uColors[MAX_COLORS];
};

uniform bool uShowPatches;
uniform int uNumPatches;

flat in float tcPatch;

void main()
{

    if (uShowPatches)
    {
        int patchIndex = int(floor(tcPatch));
        oColor = uColors[patchIndex];
    }
    else
    {
        oColor = vec4(1, 1, 1, 1);
    }
}