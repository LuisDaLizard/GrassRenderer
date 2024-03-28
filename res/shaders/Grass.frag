#version 400 core

out vec4 oColor;

in float fHeight;

void main()
{
    oColor = fHeight * vec4(0, 1, 0, 1);
}