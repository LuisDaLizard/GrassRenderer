#version 400 core

out vec4 oColor;

uniform vec3 uLightPos;

in vec3 fPosition;
in vec3 fNormal;

float diffuse(vec3 lightDir, vec3 normal)
{
    return max(dot(lightDir, normal), 0.0);
}
void main()
{
    vec3 lightDir = normalize(uLightPos - fPosition);

    vec4 diffuse = vec4(vec3(diffuse(lightDir, fNormal)), 1);
    vec4 ambience = vec4(vec3(0.1), 1);

    oColor = ambience + diffuse;
}