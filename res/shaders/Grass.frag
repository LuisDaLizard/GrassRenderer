#version 450

#define MAX_COLORS 256

layout(location = 0) out vec4 oColor;

//layout (std140) uniform Colors
//{
//    vec4 uColors[MAX_COLORS];
//};

//uniform bool uShowPatches;
//uniform int uNumPatches;

layout(location = 0) flat in float tePatch;
layout(location = 1) in float teHeight;

void main()
{

//    if (uShowPatches)
//    {
//        int patchIndex = int(floor(tePatch));
//        oColor = uColors[patchIndex];
//    }
//    else
    {
        oColor = vec4(0, teHeight, 0, 1);
    }
}