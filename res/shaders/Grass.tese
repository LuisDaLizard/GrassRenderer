#version 450

layout(quads, equal_spacing, ccw) in;

layout(location = 0) patch in float tcPatch;
layout(location = 1) patch in vec3 tcV1;
layout(location = 2) patch in vec3 tcV2;
layout(location = 3) patch in vec3 tcBladeDir;
layout(location = 4) patch in vec3 tcBladeUp;
layout(location = 5) patch in vec3 tcBladeBitangent;
layout(location = 6) patch in float tcWidth;

layout(binding = 1) uniform MatrixUniforms
{
    mat4 uProj;
    mat4 uView;
    mat4 uWorld;
} Uniforms;

layout(location = 0) flat out float tePatch;
layout(location = 1) out float teHeight;

vec3 interpolate(vec3 curvePoint1, vec3 curvePoint2, float t)
{
    return mix(curvePoint1, curvePoint2, t);
}

void main()
{
    float u = gl_TessCoord.x;
    float v = gl_TessCoord.y;

    vec3 offset = (tcBladeDir * tcWidth) * 0.5;

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
    gl_Position = Uniforms.uProj * Uniforms.uView * vec4(position, 1);
}