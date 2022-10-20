#version 430

layout (location = 0) in vec3 iNormal;
layout (location = 1) in vec2 iUV;

layout (location = 0) out vec4 oColor;

layout (binding = 0, set = 1) uniform sampler2D uTexture;

void main() {
    oColor = texture(uTexture, iUV);
}