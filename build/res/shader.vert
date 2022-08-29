#version 430 core

layout (location = 0) in vec3 Positions;
layout (location = 1) in vec4 Colors;
layout (location = 2) in vec2 TexCoords;

layout (location = 0) out vec4 Color;
layout (location = 1) out vec2 TexCoord;

/*
layout (binding = 0) uniform Shit {
    mat4 mvp;
} data;
*/
void main() {
    Color = Colors;
    TexCoord = TexCoords;
    gl_Position = vec4(Positions, 1);
}