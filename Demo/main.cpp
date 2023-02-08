#pragma once
#include <QApplication>
#include <QWindow>
#include "../Runtime/Core/Bootstrapper.hpp"
#include <iostream>
#include "../Runtime/Core/Manager/GraphicDeviceManager.hpp"
#include "../Runtime/Core/Object/Transform.hpp"
#include <qdebug.h>

int main(int argc, char* argv[])
{
    QApplication app(argc, argv);

    AirEngine::Runtime::Core::Object::Transform root{};
    AirEngine::Runtime::Core::Object::Transform a0{};
    AirEngine::Runtime::Core::Object::Transform a1{};
    AirEngine::Runtime::Core::Object::Transform a2{};

    AirEngine::Runtime::Core::Object::Object& obj = root;
    auto s = obj.Type().get_name().to_string();

    //root.AddChild(&a0);
    //root.AddChild(&a1);
    //root.AddChild(&a2);

    //root.AddChild(&a0);
    //a0.AddElderBrother(&a1);
    //a0.AddElderBrother(&a2);

    //root.AddChild(&a0);
    //a0.AddYoungerBrother(&a1);
    //a0.AddYoungerBrother(&a2);

    root.AddChild(&a0);
    a0.AddEldestBrother(&a1);
    a0.AddYoungestBrother(&a2);

    for (auto iter = a0.BrotherIterator(); iter; ++iter)
    {
        std::cout << iter->Scale().x << std::endl;
    }

    QWindow* window = new QWindow();
    window->show();    

    std::unique_ptr<AirEngine::Runtime::Core::Bootstrapper> bootstrapper = std::unique_ptr<AirEngine::Runtime::Core::Bootstrapper>(new AirEngine::Runtime::Core::Bootstrapper());
    bootstrapper->Boot();

    return app.exec();
}