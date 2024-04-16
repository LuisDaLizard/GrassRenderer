#version 450

layout (location = 0) in vec4 aV0;
layout (location = 1) in vec4 aV1;
layout (location = 2) in vec4 aV2;
layout (location = 3) in vec4 aAttribs;

layout(location = 0) flat out vec4 vV1;
layout(location = 1) flat out vec4 vV2;
layout(location = 2) flat out vec3 vBladeUp;
layout(location = 3) flat out vec3 vBladeDir;

void main()
{
    vBladeUp = aAttribs.xyz;
    vV1 = aV1;
    vV2 = aV2;
    float dir = aV0.w;
    float sd = sin(dir);
    float cd = cos(dir);
    vec3 temp = normalize(vec3(sd, sd + cd, cd));
    vBladeDir = normalize(cross(vBladeUp, temp));

    gl_Position = vec4(aV0.xyz, 1);
}