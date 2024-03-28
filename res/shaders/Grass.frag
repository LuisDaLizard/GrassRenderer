#version 400 core

#define MAX_COLORS 256

out vec4 oColor;

layout (std140) uniform Colors
{
    vec4 uColors[MAX_COLORS];
};

uniform bool uShowPatches;
uniform int uNumPatches;

in float fPatch;
in float fHeight;

void main()
{

    if (uShowPatches)
    {
        int patchIndex = int(floor(fPatch));
        oColor = uColors[patchIndex];
    }
    else
    {
        oColor = fHeight * vec4(0, 1, 0, 1);
    }
}