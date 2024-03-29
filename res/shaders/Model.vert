#version 400 core

layout(location = 0) in vec3 aPosition;
layout(location = 1) in vec3 aNormal;

uniform mat4 uProjection;
uniform mat4 uView;
uniform mat4 uWorld;

out vec3 fNormal;
out vec3 fPosition;

void main()
{
    fNormal = aNormal;
    fPosition = (uWorld * vec4(aPosition, 1)).xyz;
    gl_Position = uProjection * uView * uWorld * vec4(aPosition, 1);
}