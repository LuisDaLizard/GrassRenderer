#version 450

layout(vertices = 1) out;

layout(location = 0) flat in vec4 vV1[];
layout(location = 1) flat in vec4 vV2[];
layout(location = 2) flat in vec3 vBladeUp[];
layout(location = 3) flat in vec3 vBladeDir[];

layout(location = 0) patch out vec4 tcV1;
layout(location = 1) patch out vec4 tcV2;
layout(location = 2) patch out vec3 tcBladeUp;
layout(location = 3) patch out vec3 tcBladeDir;

void main()
{
    gl_out[gl_InvocationID].gl_Position = gl_in[gl_InvocationID].gl_Position;

    tcV1 = vV1[0];
    tcV2 = vV2[0];
    tcBladeUp = vBladeUp[0];
    tcBladeDir = vBladeDir[0];

    gl_TessLevelInner[0] = 1;
    gl_TessLevelInner[1] = 10;

    gl_TessLevelOuter[0] = 10;
    gl_TessLevelOuter[1] = 1;
    gl_TessLevelOuter[2] = 10;
    gl_TessLevelOuter[3] = 1;
}