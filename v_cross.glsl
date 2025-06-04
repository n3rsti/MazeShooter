#version 330

in vec4 vertex;
in vec4 color;
in vec2 texCoord0;

uniform mat4 M;
uniform mat4 V;
uniform mat4 P;

out vec4 ic;
out vec2 iTexCoord0;

void main(void) {
    gl_Position = P * V * M * vertex;
    ic = color;
    iTexCoord0 = texCoord0;
}
