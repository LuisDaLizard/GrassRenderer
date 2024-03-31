#version 400 core

layout (quads, equal_spacing, ccw) in;

patch in float tcPatch;
patch in vec3 tcV1;
patch in vec3 tcV2;
patch in vec3 tcBladeDir;
patch in vec3 tcBladeUp;
patch in vec3 tcBladeBitangent;

uniform mat4 uProjection;
uniform mat4 uView;
uniform mat4 uWorld;
uniform float uHeight;
uniform float uWidth;

flat out float tePatch;
out float teHeight;

vec3 interpolate(vec3 curvePoint1, vec3 curvePoint2, float t)
{
    return mix(curvePoint1, curvePoint2, t);
}

void main()
{
    float u = gl_TessCoord.x;
    float v = gl_TessCoord.y;

    vec3 offset = (tcBladeDir * uWidth) * 0.5;

    vec3 v0 = vec3(gl_in[0].gl_Position) - offset;
    vec3 v1 = tcV1 - offset;
    vec3 v2 = tcV2 - offset;

    vec3 a = v0 + v * (v1 - v0);
    vec3 b = v1 + v * (v2 - v1);
    vec3 c = a + v * (b - a);

    vec3 c0 = c;
    vec3 c1 = c + offset * 2;

    vec3 bitangent = tcBladeBitangent;
    vec3 tangent = normalize(b - a);
    vec3 normal = normalize(cross(tangent, bitangent));

    float t = u - pow(v, 2.0) * u;

    vec3 position = mix(c0, c1, t);

    teHeight = v;
    tePatch = tcPatch;
    gl_Position = uProjection * uView * vec4(position, 1);
}