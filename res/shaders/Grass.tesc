#version 400 core

layout (vertices = 1) out;

flat in float vPatch[];
flat in vec3 vV1[];
flat in vec3 vV2[];
flat in vec3 vBladeDir[];
flat in vec3 vBladeUp[];

patch out float tcPatch;
patch out vec3 tcV1;
patch out vec3 tcV2;
patch out vec3 tcBladeDir;
patch out vec3 tcBladeUp;
patch out vec3 tcBladeBitangent;

void main()
{
    gl_out[gl_InvocationID].gl_Position = gl_in[gl_InvocationID].gl_Position;

    tcPatch = vPatch[0];
    tcV1 = vV1[0];
    tcV2 = vV2[0];
    tcBladeDir = vBladeDir[0];
    tcBladeUp = vBladeUp[0];
    tcBladeBitangent = vBladeDir[0];

    gl_TessLevelInner[0] = 1;
    gl_TessLevelInner[1] = 10;

    gl_TessLevelOuter[0] = 10;
    gl_TessLevelOuter[1] = 1;
    gl_TessLevelOuter[2] = 10;
    gl_TessLevelOuter[3] = 1;
}