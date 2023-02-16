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
#include "../Runtime/Utility/Fiber.hpp"
#include <sstream>

using namespace AirEngine::Runtime;
using namespace AirEngine::Runtime::Utility;
static constexpr int WORKER_THREAD_COUNT = 16;
static std::mutex endMutex{};
static Fiber::condition_variable_any endConditionVariable{};
static bool isEnd = false;
static std::array< std::thread*, WORKER_THREAD_COUNT> workerThreads;   
static std::thread* mainThread;


void Test() {
    //std::cout << "main thread started " << std::this_thread::get_id() << std::endl;
    auto hardwareThreadCount = std::thread::hardware_concurrency();
    auto showThreadIdTask = [](const std::string& name)->void
    {
        std::stringstream ss{};
        ss << name << ": " << std::this_thread::get_id() << ".\n";
        std::cout << ss.str();
    };
    auto worker = []()->void
    {
        Fiber::use_scheduling_algorithm< Fiber::algo::work_stealing >(WORKER_THREAD_COUNT + 1);
        std::unique_lock<std::mutex> lock(endMutex);
        endConditionVariable.wait(lock, []() { return isEnd; });
    };
    auto mainLoopTask = [&showThreadIdTask]()->void
    {
        while (true)
        {
            ThisFiber::yield();
            showThreadIdTask("MainLoop");
            std::array< Fiber::fiber, 40> perFrameTasks;
            for (int i = 0; i < perFrameTasks.size(); i++)
            {
                perFrameTasks[i] = Fiber::fiber(showThreadIdTask, std::to_string(i));
            }
            ThisFiber::yield();
            ThisFiber::yield();
            ThisFiber::yield();
            ThisFiber::yield();
            ThisFiber::yield();
            for (int i = 0; i < perFrameTasks.size(); i++)
            {
                perFrameTasks[i].join();
            }
        }
    };
    auto leader = [&mainLoopTask, &showThreadIdTask]()->void
    {
        Fiber::fiber(mainLoopTask).detach();

        Fiber::use_scheduling_algorithm< Fiber::algo::work_stealing >(WORKER_THREAD_COUNT + 1);
        std::unique_lock<std::mutex> lock(endMutex);
        endConditionVariable.wait(lock, []() { return isEnd; });
        std::cout << "End leader.\n";
    };

    mainThread = new std::thread(leader);
    for (auto& workerThread : workerThreads)
    {
        workerThread = new std::thread(worker);
    }

    //while (true)
    //{
    //    if (std::cin.)
    //    {
    //        {
    //            std::unique_lock<std::mutex> lock(endMutex);
    //            isEnd = true;
    //        }
    //        endConditionVariable.notify_all();
    //        for (std::thread& t : threads) {
    //            t.join();
    //        }
    //        break;
    //    }
    //    std::this_thread::yield();
    //}
}
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

    Test();

    QWindow* window = new QWindow();
    window->show();    
    app.exec();

}