#version 330 core

layout(location = 0) in vec4 vertex;
layout(location = 1) in vec4 color;

uniform mat4 M;
uniform mat4 V; // tu możesz zawsze dać tożsamość
uniform mat4 P; // tu też tożsamość

out vec4 fragColor;

void main() {
    gl_Position = P * V * M * vertex;
    fragColor = color;
}
