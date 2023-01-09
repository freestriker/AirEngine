#pragma once
#include <QApplication>
#include <qwindow.h>
#include "../Runtime/Core/Context.hpp"
#include <iostream>
#include "../Runtime/Core/Manager/GraphicDeviceManager.hpp"
using namespace std;
int main(int argc, char* argv[])
{
    QApplication app(argc, argv);

    QWindow* window = new QWindow();
    window->show();

    AirEngine::Runtime::Core::Context::Init();
    return app.exec();
}