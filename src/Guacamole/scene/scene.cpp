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

#include <Guacamole/core/application.h>
#include <Guacamole/vulkan/swapchain.h>

namespace Guacamole {

Scene::Scene(Application* app) : mApplication(app) {
    mRenderer = new SceneRenderer(this);    
}

Scene::~Scene() {
    delete mRenderer;
    mRegistry.clear();
}

void Scene::OnUpdate(float ts) {
    auto view = mRegistry.view<ScriptComponent>();

    for (auto entity : view) {
        Entity e(entity, this);

        ScriptComponent& script = view.get<ScriptComponent>(entity);

        script.mScript(&e, ts);
    }
}

void Scene::OnRender() {
    mRenderer->Begin();

    Camera cam;

    auto cameraView = mRegistry.view<CameraComponent, TransformComponent>();

    for (auto entity : cameraView) {
        CameraComponent& c = cameraView.get<CameraComponent>(entity);
        TransformComponent& trans = cameraView.get<TransformComponent>(entity);

        c.mCamera.SetView(trans.GetView());

        if (c.mPrimary) {
            cam = c.mCamera;
        }
    }

    auto view = mRegistry.view<TransformComponent, MeshComponent, MaterialComponent>();

    mRenderer->BeginScene(cam);

    for (auto entity : view) {
        const TransformComponent& transform = view.get<TransformComponent>(entity);
        const MeshComponent& mesh = view.get<MeshComponent>(entity);
        const MaterialComponent& material = view.get<MaterialComponent>(entity);

        mRenderer->SubmitMesh(mesh, transform, material);        
    }

    mRenderer->EndScene();
    mRenderer->End();
}

Entity Scene::CreateEntity(const std::string& name) {
    Entity ent(mRegistry.create(), this);

    ent.AddComponent<IdComponent>();

    if (!name.empty()) {
        ent.AddComponent<TagComponent>(name);
    }

    return ent;
}

}