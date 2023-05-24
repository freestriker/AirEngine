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
#include "../Runtime/Utility/InternedString.hpp"
#include "../Runtime/Utility/Fiber.hpp"
#include <sstream>
#include "../Runtime/Graphic/Instance/Buffer.hpp"
#include "../Runtime/Graphic/Instance/Memory.hpp"
#include "../Runtime/Graphic/Instance/Image.hpp"

using namespace AirEngine::Runtime;
using namespace AirEngine::Runtime::Utility;
using namespace AirEngine::Runtime::Core::Manager;
using namespace AirEngine::Runtime::Graphic::Instance;
void Test();

int main(int argc, char* argv[])
{
    QApplication app(argc, argv);

    {
        std::unique_ptr<AirEngine::Runtime::Core::Bootstrapper> bootstrapper = std::unique_ptr<AirEngine::Runtime::Core::Bootstrapper>(new AirEngine::Runtime::Core::Bootstrapper());
        bootstrapper->Boot();
    }
    //std::this_thread::sleep_for(std::chrono::seconds(20));
    app.exec();

    //{
    //    std::string stdString = "stdString";
    //    std::string_view stdStringView = "stdStringView";
    //    const char* constCharPtr = "constCharPtr";

    //    InternedString is0 = InternedString(stdString);
    //    InternedString is1 = InternedString(stdStringView);
    //    InternedString is2 = InternedString(constCharPtr);

    //    bool isNull = is0.IsNULL();
    //    auto size = is0.Size();
    //    bool same = is0 == is1;
    //    bool less = is0 < is1;
    //    auto hash = std::hash<InternedString>()(is0);
    //}

    //auto&& mainScene = Core::Manager::SceneManager::Scene("MainScene");
    //auto&& rootSceneObject = mainScene.RootSceneObject();
    //rootSceneObject.SetScale({ 2, 2, 2 });

    //Core::Scene::SceneObject* sceneObject0 = new Core::Scene::SceneObject("0", true);
    //Core::Scene::SceneObject* sceneObject1 = new Core::Scene::SceneObject("1", true);
    //Core::Scene::SceneObject* sceneObject2 = new Core::Scene::SceneObject("2", true);
    //Core::Scene::SceneObject* sceneObject00 = new Core::Scene::SceneObject("00", false);
    //Core::Scene::SceneObject* sceneObject01 = new Core::Scene::SceneObject("01", false);
    //Core::Scene::SceneObject* sceneObject000 = new Core::Scene::SceneObject("000", true);

    //sceneObject00->AddChild(*sceneObject000);
    //
    //sceneObject00->SetScale({ 5, 5, 5 });

    //sceneObject0->AddChild(*sceneObject00);
    //sceneObject0->AddChild(*sceneObject01);

    //sceneObject0->SetScale({ 3, 3, 3 });

    //rootSceneObject.AddChild(*sceneObject0);
    //rootSceneObject.AddChild(*sceneObject1);
    //rootSceneObject.AddChild(*sceneObject2);

    //sceneObject0->SetScale({ 2, 2, 2 });

    //std::shared_ptr<Buffer> buffer(new Buffer(
    //    65536, 
    //    VK_BUFFER_USAGE_VERTEX_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT, 
    //    VkMemoryPropertyFlagBits::VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, 
    //    VmaAllocationCreateFlagBits::VMA_ALLOCATION_CREATE_HOST_ACCESS_SEQUENTIAL_WRITE_BIT
    //));
    //std::shared_ptr<Buffer> pureBuffer(new Buffer(65536, VK_BUFFER_USAGE_VERTEX_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT));
    //auto data = buffer->Memory()->Map();
    //std::vector<int> src(10, 5);
    //memcpy(data, src.data(), sizeof(int) * src.size());
    //buffer->Memory()->Unmap();
    //pureBuffer->SetMemory(buffer->Memory());

    //std::shared_ptr<Image> image(new Image(
    //    VK_FORMAT_B8G8R8A8_SRGB, 
    //    {128, 128, 1}, 
    //    VkImageType::VK_IMAGE_TYPE_2D, 
    //    1, 1,
    //    VkImageUsageFlagBits::VK_IMAGE_USAGE_SAMPLED_BIT | VkImageUsageFlagBits::VK_IMAGE_USAGE_TRANSFER_DST_BIT,
    //    VkMemoryPropertyFlagBits::VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT
    //));


}

static constexpr int WORKER_THREAD_COUNT = 16;
static std::mutex endMutex{};
static Fiber::condition_variable_any endConditionVariable{};
static bool isEnd = false;
static std::array< std::thread, WORKER_THREAD_COUNT> workerThreads;
static std::thread mainThread{};
static Fiber::fiber mainLoopFiber{};
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
        mainLoopFiber = std::move(Fiber::fiber(mainLoopTask));

        Fiber::use_scheduling_algorithm< Fiber::algo::work_stealing >(WORKER_THREAD_COUNT + 1);
        std::unique_lock<std::mutex> lock(endMutex);
        endConditionVariable.wait(lock, []() { return isEnd; });
        std::cout << "End leader.\n";
    };

    mainThread = std::move(std::thread(leader));
    for (auto& workerThread : workerThreads)
    {
        workerThread = std::move(std::thread(worker));
    }
}
