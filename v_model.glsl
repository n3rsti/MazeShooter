#version 330 core

uniform mat4 P;
uniform mat4 V;
uniform mat4 M;
uniform vec3 cameraPos;

layout(location = 0) in vec3 vertex;
layout(location = 1) in vec3 normal;
layout(location = 2) in vec2 texCoord0;
layout(location = 3) in vec3 tangent;
layout(location = 4) in vec3 bitangent;

out vec4 ic;
out vec4 n;
out vec4 l;
out vec4 v;
out vec2 iTexCoord0;

void main() {
    vec3 posWorld = vec3(M * vec4(vertex, 1.0));
    vec3 normalWorld = normalize(mat3(M) * normal);
    vec3 tangentWorld = normalize(mat3(M) * tangent);
    vec3 bitangentWorld = normalize(mat3(M) * bitangent);

    mat3 TBN = mat3(tangentWorld, bitangentWorld, normalWorld);

    vec3 lightVec = cameraPos - posWorld;
    vec3 viewVec = cameraPos - posWorld;

    l = vec4(TBN * lightVec, 0.0);
    v = vec4(TBN * viewVec, 0.0);
    n = vec4(0.0, 0.0, 1.0, 0.0); // base TS normal
    ic = vec4(1.0); // optional color, unused in fragment currently
    iTexCoord0 = texCoord0;

    gl_Position = P * V * M * vec4(vertex, 1.0);
}
