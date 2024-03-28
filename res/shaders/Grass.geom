#version 400 core

layout(points) in;
layout(line_strip, max_vertices = 2) out;

in vec3 gNormal[];

uniform float uHeight;
uniform mat4 uProjection;
uniform mat4 uView;
uniform mat4 uWorld;

out float fHeight;

void main()
{
    vec4 bottom = gl_in[0].gl_Position;
    vec4 top = bottom;
    top.y = uHeight;

    fHeight = 0;
    gl_Position = uProjection * uView * uWorld * bottom;
    EmitVertex();

    fHeight = 1;
    gl_Position = uProjection * uView * uWorld * top;
    EmitVertex();

    EndPrimitive();
}