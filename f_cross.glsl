#version 330

in vec4 ic;
in vec2 iTexCoord0;

uniform sampler2D tex;
uniform bool useTexture;

out vec4 fragColor;

void main(void) {
    if (useTexture)
        fragColor = texture(tex, iTexCoord0);
    else
        fragColor = ic;
}