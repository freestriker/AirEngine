#pragma once
#include <QApplication>
#include <QWindow>
#include "../Runtime/Core/Boot/Bootstrapper.hpp"
#include <iostream>
#include "../Runtime/Core/Manager/GraphicDeviceManager.hpp"
#include "../Runtime/Core/Manager/SceneManager.hpp"
#include "../Runtime/Core/Scene/SceneObject.hpp"
#include "../Runtime/Core/Scene/Scene.hpp"
#include <qdebug.h>
#include "../Runtime/Utility/InternedString.hpp"

using namespace AirEngine::Runtime;
using namespace AirEngine::Runtime::Utility;

int main(int argc, char* argv[])
{
    QApplication app(argc, argv);

    {
        std::unique_ptr<AirEngine::Runtime::Core::Boot::Bootstrapper> bootstrapper = std::unique_ptr<AirEngine::Runtime::Core::Boot::Bootstrapper>(new AirEngine::Runtime::Core::Boot::Bootstrapper());
        bootstrapper->Boot();
    }

    {
        std::string stdString = "stdString";
        std::string_view stdStringView = "stdStringView";
        const char* constCharPtr = "constCharPtr";

        InternedString is0 = InternedString(stdString);
        InternedString is1 = InternedString(stdStringView);
        InternedString is2 = InternedString(constCharPtr);

        bool isNull = is0.IsNULL();
        auto size = is0.Size();
        bool same = is0 == is1;
        bool less = is0 < is1;
        auto hash = std::hash<InternedString>()(is0);
    }

    auto&& mainScene = Core::Manager::SceneManager::Scene("MainScene");
    auto&& rootSceneObject = mainScene.RootSceneObject();
    rootSceneObject.SetScale({ 2, 2, 2 });

    Core::Scene::SceneObject* sceneObject0 = new Core::Scene::SceneObject("0", true);
    Core::Scene::SceneObject* sceneObject1 = new Core::Scene::SceneObject("1", true);
    Core::Scene::SceneObject* sceneObject2 = new Core::Scene::SceneObject("2", true);
    Core::Scene::SceneObject* sceneObject00 = new Core::Scene::SceneObject("00", false);
    Core::Scene::SceneObject* sceneObject01 = new Core::Scene::SceneObject("01", false);
    Core::Scene::SceneObject* sceneObject000 = new Core::Scene::SceneObject("000", true);

    sceneObject00->AddChild(*sceneObject000);
    
    sceneObject00->SetScale({ 5, 5, 5 });

    sceneObject0->AddChild(*sceneObject00);
    sceneObject0->AddChild(*sceneObject01);

    sceneObject0->SetScale({ 3, 3, 3 });

    rootSceneObject.AddChild(*sceneObject0);
    rootSceneObject.AddChild(*sceneObject1);
    rootSceneObject.AddChild(*sceneObject2);

    sceneObject0->SetScale({ 2, 2, 2 });

    QWindow* window = new QWindow();
    window->show();    

    return app.exec();
}