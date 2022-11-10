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

layout (binding = 0, set = 1) uniform ModelData {
    mat4 mModel;
} uModelData;

void main() {

    gl_Position = uSceneData.mProjection * uSceneData.mView * uModelData.mModel * vec4(iPosition, 1);
}

