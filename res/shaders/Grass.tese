#version 400 core

layout (quads, equal_spacing, ccw) in;

patch in float tcPatch;
patch in vec4 tcV2;
patch in vec4 tcBladeDir;
patch in vec4 tcBladeUp;

uniform mat4 uProjection;
uniform mat4 uView;
uniform mat4 uWorld;

out float tePatch;

void main()
{
    float u = gl_TessCoord.x;
    float v = gl_TessCoord.y;
    vec4 position = gl_in[0].gl_Position;
    position += (u * vec4(1, 0, 0, 0));
    position +=

    tePatch = tcPatch;
    gl_Position = uProjection * uView * position;
}