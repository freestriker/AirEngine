#include "MainManager.hpp"
#include <algorithm>
#include "GraphicDeviceManager.hpp"
#include "SceneManager.hpp"
#include "ThirdPartyLibraryManager.hpp"
#include "FiberManager.hpp"
#include "RenderManager.hpp"
#include "LogicManager.hpp"
#include "AssetManager.hpp"
#include "TaskManager.hpp"
#include <memory>
#include <taskflow/algorithm/sort.hpp>

using namespace AirEngine::Runtime;

AirEngine::Runtime::Core::Manager::MainManager::MainManager()
	: Manager::ManagerBase("MainManager")
	, _managerTable{}
	, _mainLoopFuture()
{
	ResetManager(std::make_shared<Manager::FiberManager>());
	ResetManager(std::make_shared<Manager::ThirdPartyLibraryManager>());
	ResetManager(std::make_shared<Manager::SceneManager>());
	ResetManager(std::make_shared<Manager::GraphicDeviceManager>());
	ResetManager(std::make_shared<Manager::RenderManager>());
	ResetManager(std::make_shared<Manager::LogicManager>());
	ResetManager(std::make_shared<Manager::AssetManager>());
	ResetManager(std::make_shared<Manager::TaskManager>());
}

void Core::Manager::MainManager::Initialize()
{
	std::cout << "----------Pre Initialize----------" << std::endl;
	{
		std::vector<Utility::OperationWrapper> preInitializers{};
		for (auto& managerPair : _managerTable)
		{
			auto&& temp = managerPair.second->OnGetPreInitializeOperation();
			preInitializers.emplace_back(std::move(temp));
		}

		std::sort(preInitializers.begin(), preInitializers.end(), [](const auto& x, const auto& y)->bool {
			return x.layer == y.layer ? x.index < y.index : x.layer < y.layer;
		});

		for (auto& initializer : preInitializers)
		{
			initializer.operation();
		}
	}
	std::cout << "------------------------------" << std::endl;

	std::cout << "----------Initialize----------" << std::endl;
	{
		std::vector<Utility::OperationWrapper> initializers{};
		for (auto& managerPair : _managerTable)
		{
			auto&& temp = managerPair.second->OnGetInitializeOperations();
			initializers.insert(initializers.end(), temp.begin(), temp.end());
		}

		std::sort(initializers.begin(), initializers.end(), [](const auto& x, const auto& y)->bool {
			return x.layer == y.layer ? x.index < y.index : x.layer < y.layer;
		});

		for (auto& initializer : initializers)
		{
			initializer.operation();
		}
	}
	std::cout << "------------------------------" << std::endl;

	std::cout << "----------Post Initialize----------" << std::endl;
	{
		std::vector<Utility::OperationWrapper> postInitializers{};
		for (auto& managerPair : _managerTable)
		{
			auto&& temp = managerPair.second->OnGetPostInitializeOperation();
			postInitializers.emplace_back(std::move(temp));
		}

		std::sort(postInitializers.begin(), postInitializers.end(), [](const auto& x, const auto& y)->bool {
			return x.layer == y.layer ? x.index < y.index : x.layer < y.layer;
			});

		for (auto& initializer : postInitializers)
		{
			initializer.operation();
		}
	}
	std::cout << "------------------------------" << std::endl;

	{

		std::shared_ptr<std::vector<Utility::OperationWrapper>> updaters = std::make_shared<std::vector<Utility::OperationWrapper>>();
		for (auto& managerPair : _managerTable)
		{
			auto&& temp = managerPair.second->OnGetUpdateOperations();
			updaters->insert(updaters->end(), temp.begin(), temp.end());
		}

		tf::Taskflow mainLoopTaskFlow("MainLoopTaskFlow");
		auto&& sortTask = mainLoopTaskFlow.sort(updaters->begin(), updaters->end(), [](const auto& x, const auto& y)->bool {
				return x.layer == y.layer ? x.index < y.index : x.layer < y.layer;
			}
		);
		auto&& mainLoopTask = mainLoopTaskFlow.emplace(
			[updaters]()->void
			{
				size_t frameIndex = 0;
				while (true)
				{
					//std::cout << "----------Frame " + std::to_string(frameIndex) + " Update----------" << std::endl;

					for (const auto& updater : *updaters)
					{
						updater.operation();
					}
					std::this_thread::yield();
					++frameIndex;

					//std::cout << "------------------------------" << std::endl;
				}
			}
		); 
		sortTask.succeed(mainLoopTask);

		_mainLoopFuture = Manager::TaskManager::Executor().run(std::move(mainLoopTaskFlow));
	}
}

void AirEngine::Runtime::Core::Manager::MainManager::Finalize()
{
	std::cout << "----------Pre Finalize----------" << std::endl;
	{
		std::vector<Utility::OperationWrapper> preFinalizers{};
		for (auto& managerPair : _managerTable)
		{
			auto&& temp = managerPair.second->OnGetPostFinalizeOperation();
			preFinalizers.emplace_back(std::move(temp));
		}

		std::sort(preFinalizers.begin(), preFinalizers.end(), [](const auto& x, const auto& y)->bool {
			return x.layer == y.layer ? x.index < y.index : x.layer < y.layer;
			});

		for (auto& finalizer : preFinalizers)
		{
			finalizer.operation();
		}
	}
	std::cout << "------------------------------" << std::endl;

	std::cout << "----------Finalize----------" << std::endl;
	{
		std::vector<Utility::OperationWrapper> finalizers{};
		for (auto& managerPair : _managerTable)
		{
			auto&& temp = managerPair.second->OnGetFinalizeOperations();
			finalizers.insert(finalizers.end(), temp.begin(), temp.end());
		}

		std::sort(finalizers.begin(), finalizers.end(), [](const auto& x, const auto& y)->bool {
			return x.layer == y.layer ? x.index < y.index : x.layer < y.layer;
		});

		for (auto& finalizer : finalizers)
		{
			finalizer.operation();
		}
	}
	std::cout << "------------------------------" << std::endl;

	std::cout << "----------Post Finalize----------" << std::endl;
	{
		std::vector<Utility::OperationWrapper> postFinalizers{};
		for (auto& managerPair : _managerTable)
		{
			auto&& temp = managerPair.second->OnGetPostFinalizeOperation();
			postFinalizers.emplace_back(std::move(temp));
		}

		std::sort(postFinalizers.begin(), postFinalizers.end(), [](const auto& x, const auto& y)->bool {
			return x.layer == y.layer ? x.index < y.index : x.layer < y.layer;
			});

		for (auto& finalizer : postFinalizers)
		{
			finalizer.operation();
		}
	}
	std::cout << "------------------------------" << std::endl;
}

void AirEngine::Runtime::Core::Manager::MainManager::ResetManager(std::shared_ptr<Manager::ManagerBase> manager)
{
	_managerTable[manager->Name()] = manager;
}
