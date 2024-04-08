#version 450

layout(location = 0) out vec4 oColor;

layout(location = 0) in vec3 fPosition;
layout(location = 1) in vec3 fNormal;

float diffuse(vec3 lightDir, vec3 normal)
{
    return max(dot(lightDir, normal), 0.0);
}
void main()
{
    vec3 lightPos = vec3(0, 2, 0);
    vec3 lightDir = normalize(lightPos - fPosition);

    vec4 diffuse = vec4(vec3(diffuse(lightDir, fNormal)), 1);
    vec4 ambience = vec4(vec3(0.1), 1);

    oColor = ambience + diffuse;
}