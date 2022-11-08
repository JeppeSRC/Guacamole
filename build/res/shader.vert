#version 430 core

layout (location = 0) in vec4 Positions;
layout (location = 1) in vec3 Normals;
layout (location = 2) in vec2 TexCoords;

layout (location = 0) out vec3 Normal;
layout (location = 1) out vec2 TexCoord;


layout (binding = 2) uniform Shit {
    mat4 Model;
    mat4 View;
    mat4 Projection;
} data;

void main() {
    Normal = Normals;
    TexCoord = TexCoords;
    gl_Position = data.Projection * data.View * data.Model * vec4(Positions.xyz, 1);
}