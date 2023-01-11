#pragma once
#include <QApplication>
#include <QWindow>
#include "../Runtime/Core/Bootstrapper.hpp"
#include <iostream>
#include "../Runtime/Core/Manager/GraphicDeviceManager.hpp"
#include <qdebug.h>

int main(int argc, char* argv[])
{
    QApplication app(argc, argv);

    QWindow* window = new QWindow();
    window->show();    

    std::unique_ptr<AirEngine::Runtime::Core::Bootstrapper> bootstrapper = std::unique_ptr<AirEngine::Runtime::Core::Bootstrapper>(new AirEngine::Runtime::Core::Bootstrapper());
    bootstrapper->Boot();

    return app.exec();
}