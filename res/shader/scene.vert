#version 430

layout (location = 0) in vec3 iPosition;
layout (location = 1) in vec3 iNormal;
layout (location = 2) in vec2 iUV;

layout (location = 0) out vec3 oNormal;
layout (location = 1) out vec2 oUV;

layout (binding = 0, set = 0) uniform SceneData {
    mat4 mProjection;
    mat4 mView;
} uSceneData;

void main() {

    gl_Position = mProjection * mView * vec4(iPosition, 1);
}

