#pragma once
#include <QApplication>
#include <QWindow>
#include "../Runtime/Core/Bootstrapper.hpp"
#include <iostream>
#include "../Runtime/Core/Manager/GraphicDeviceManager.hpp"
#include "../Runtime/Core/Manager/SceneManager.hpp"
#include "../Runtime/Core/Scene/SceneObject.hpp"
#include "../Runtime/Core/Scene/Scene.hpp"
#include <qdebug.h>

using namespace AirEngine::Runtime;

int main(int argc, char* argv[])
{
    QApplication app(argc, argv);

    {
        std::unique_ptr<AirEngine::Runtime::Core::Bootstrapper> bootstrapper = std::unique_ptr<AirEngine::Runtime::Core::Bootstrapper>(new AirEngine::Runtime::Core::Bootstrapper());
        bootstrapper->Boot();
    }

    auto&& mainScene = Core::Manager::SceneManager::Scene("MainScene");
    auto&& rootSceneObject = mainScene.RootSceneObject();
    rootSceneObject.SetScale({ 2, 2, 2 });

    Core::Scene::SceneObject* sceneObject0 = new Core::Scene::SceneObject("0");
    Core::Scene::SceneObject* sceneObject1 = new Core::Scene::SceneObject("1");
    Core::Scene::SceneObject* sceneObject2 = new Core::Scene::SceneObject("2");
    Core::Scene::SceneObject* sceneObject00 = new Core::Scene::SceneObject("00");
    Core::Scene::SceneObject* sceneObject01 = new Core::Scene::SceneObject("01");
    Core::Scene::SceneObject* sceneObject000 = new Core::Scene::SceneObject("000");

    sceneObject0->AddChild(*sceneObject00);
    sceneObject0->AddChild(*sceneObject01);

    sceneObject00->AddChild(*sceneObject000);

    sceneObject0->SetScale({ 3, 3, 3 });

    rootSceneObject.AddChild(*sceneObject0);
    rootSceneObject.AddChild(*sceneObject1);
    rootSceneObject.AddChild(*sceneObject2);

    sceneObject0->SetScale({ 2, 2, 2 });

    QWindow* window = new QWindow();
    window->show();    

    return app.exec();
}