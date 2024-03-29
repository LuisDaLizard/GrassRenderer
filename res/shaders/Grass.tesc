#version 400 core

layout (vertices = 1) out;

flat in float vPatch[];
flat in vec4 vV2[];
flat in vec4 vBladeDir[];
flat in vec4 vBladeUp[];

patch out float tcPatch;
patch out vec4 tcV2;
patch out vec4 tcBladeDir;
patch out vec4 tcBladeUp;

void main()
{
    gl_out[gl_InvocationID].gl_Position = gl_in[gl_InvocationID].gl_Position;

    tcPatch = vPatch[0];
    tcV2 = vV2[0];
    tcBladeDir = vBladeDir[0];
    tcBladeUp = vBladeUp[0];

    gl_TessLevelInner[0] = 0;
    gl_TessLevelInner[1] = 0;

    gl_TessLevelOuter[0] = 1;
    gl_TessLevelOuter[1] = 1;
    gl_TessLevelOuter[2] = 1;
    gl_TessLevelOuter[3] = 1;
}