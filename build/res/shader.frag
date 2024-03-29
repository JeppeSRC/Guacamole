#version 430 core

layout (location = 0) in vec3 Normal;
layout (location = 1) in vec2 TexCoord;

layout (location = 0) out vec4 Color;

layout (binding = 0) uniform Buffer {
    vec4 Color;
} data;

layout (binding = 1) uniform sampler2D Texture;


void main() {

    Color = data.Color * texture(Texture, TexCoord);
}