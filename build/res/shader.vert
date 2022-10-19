#version 430 core

layout (location = 0) in vec4 Positions;
layout (location = 1) in vec3 Normals;
layout (location = 2) in vec2 TexCoords;

layout (location = 0) out vec3 Normal;
layout (location = 1) out vec2 TexCoord;

/*
layout (binding = 0) uniform Shit {
    mat4 mvp;
} data;
*/
void main() {
    Normal = Normals;
    TexCoord = TexCoords;
    gl_Position = vec4(Positions.xyz, 1);
}