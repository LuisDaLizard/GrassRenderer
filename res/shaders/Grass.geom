#version 400 core

layout(triangles) in;
layout(line_strip, max_vertices = 2) out;

in float gPatch[];
in vec3 gNormal[];

uniform mat4 uProjection;
uniform mat4 uView;
uniform mat4 uWorld;

out float fPatch;
out float fHeight;

void main()
{
    vec4 bottom = vec4(vec3(gl_in[0].gl_Position), 1);
    vec4 top = vec4(gNormal[0], 1);

    fPatch = gPatch[0] - 0.01;
    fHeight = 0;
    gl_Position = uProjection * uView * uWorld * bottom;
    EmitVertex();

    fPatch = gPatch[0] - 0.01;
    fHeight = 1;
    gl_Position = uProjection * uView * uWorld * top;
    EmitVertex();

    EndPrimitive();
}