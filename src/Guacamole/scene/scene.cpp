/*
MIT License

Copyright (c) 2022 Jesper

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
*/

#include <Guacamole.h>

#include "scene.h"
#include "entity.h"

#include <Guacamole/vulkan/context.h>

namespace Guacamole {

Scene::Scene(const std::filesystem::path& path) : Asset(path, AssetType::Scene) {
    mRenderer = new SceneRenderer(1280, 720);    


}

Scene::~Scene() {
    mRegistry.clear();
}


void Scene::OnUpdate(float ts) {
    mRenderer->Begin(this);
}

void Scene::OnRender() {
    auto cameraView = mRegistry.view<CameraComponent>();

    GM_ASSERT(cameraView.size());

    Camera cam;

    for (auto entity : cameraView) {
        CameraComponent c = cameraView.get<CameraComponent>(entity);

        if (c.mPrimary) {
            cam = c.mCamera;
            break;
        }
    }

    GM_ASSERT(cam.GetProjection() == glm::mat4(1.0f));
    
    mRenderer->BeginScene(cam);
    StagingBuffer* staging = mRenderer->GetStagingBuffer();

    auto view = mRegistry.view<TransformComponent, MeshComponent, MaterialComponent>();

    for (auto entity : view) {
        const TransformComponent& transform = view.get<TransformComponent>(entity);
        const MeshComponent& mesh = view.get<MeshComponent>(entity);
        const MaterialComponent& mat = view.get<MaterialComponent>(entity);

        
        
    }

    mRenderer->EndScene();
}

Entity Scene::CreateEntity(const std::string& name) {
    Entity ent(this, mRegistry.create());

    ent.AddComponent<IdComponent>();

    if (!name.empty()) {
        ent.AddComponent<TagComponent>(name);
    }

    return ent;
}



void Scene::Load() {
    GM_ASSERT(false);
}

void Scene::Unload() {
    GM_ASSERT(false);
}



}