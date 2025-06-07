#version 330 core

uniform mat4 P;
uniform mat4 V;
uniform mat4 M;

layout(location = 0) in vec3 vertex;
layout(location = 1) in vec3 normal;
layout(location = 2) in vec2 texCoord0;
layout(location = 3) in vec3 tangent;
layout(location = 4) in vec3 bitangent;

out vec2 iTexCoord0;
out vec3 lightDirTS;
out vec3 viewDirTS;

void main() {
    vec3 posWorld = vec3(M * vec4(vertex, 1.0));
    vec3 normalWorld = normalize(mat3(M) * normal);
    vec3 tangentWorld = normalize(mat3(M) * tangent);
    vec3 bitangentWorld = normalize(mat3(M) * bitangent);

    mat3 TBN = mat3(tangentWorld, bitangentWorld, normalWorld);

    vec3 lightPosWorld = vec3(0.0, 0.0, -6.0);
    vec3 viewPosWorld = vec3(0.0, 0.0, 0.0);

    vec3 lightVec = lightPosWorld - posWorld;
    vec3 viewVec = viewPosWorld - posWorld;

    lightDirTS = normalize(TBN * lightVec);
    viewDirTS = normalize(TBN * viewVec);

    iTexCoord0 = texCoord0;

    gl_Position = P * V * M * vec4(vertex, 1.0);
}
