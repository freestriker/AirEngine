#include "MainManager.hpp"
#include <algorithm>
#include "RenderManager.hpp"
#include "SceneManager.hpp"
#include "ThirdPartyLibraryManager.hpp"
#include "LogicManager.hpp"
#include "AssetManager.hpp"
#include "TaskManager.hpp"
#include <memory>
#include <taskflow/algorithm/sort.hpp>
#include "AirEngine/Runtime/Graphic/Manager/DescriptorManager.hpp"

using namespace AirEngine::Runtime;

AirEngine::Runtime::Core::Manager::MainManager::MainManager()
	: Manager::ManagerBase("MainManager")
	, _managerTable{}
	, _mainLoopFuture()
{
	ResetManager(std::make_shared<Manager::ThirdPartyLibraryManager>());
	ResetManager(std::make_shared<Manager::SceneManager>());
	ResetManager(std::make_shared<Manager::RenderManager>());
	ResetManager(std::make_shared<Manager::LogicManager>());
	ResetManager(std::make_shared<Manager::AssetManager>());
	ResetManager(std::make_shared<Manager::TaskManager>());
	ResetManager(std::make_shared<Graphic::Manager::DescriptorManager>());
}

void Core::Manager::MainManager::Initialize()
{
	std::cout << "----------Pre Initialize----------" << std::endl;
	{
		std::vector<std::pair<Utility::OperationWrapper, std::string>> preInitializers{};
		for (auto& managerPair : _managerTable)
		{
			auto&& temp = managerPair.second->OnGetPreInitializeOperation();
			if (!temp.operation) continue;
			preInitializers.emplace_back(std::pair<Utility::OperationWrapper, std::string>(std::move(temp), managerPair.second->Name()));
		}

		std::sort(preInitializers.begin(), preInitializers.end(), [](const auto& x, const auto& y)->bool {
			return x.first.layer == y.first.layer ? x.first.index < y.first.index : x.first.layer < y.first.layer;
		});

		for (auto& initializer : preInitializers)
		{
			initializer.first.operation();
			std::cout << "Layer: " << initializer.first.layer << "\tIndex: " << initializer.first.index << "\tManager: " << initializer.second << "\tDescription: " << initializer.first.description << std::endl;
		}

	}
	std::cout << "------------------------------" << std::endl;

	std::cout << "----------Initialize----------" << std::endl;
	{
		std::vector<std::pair<Utility::OperationWrapper, std::string>> initializers{};
		for (auto& managerPair : _managerTable)
		{
			auto&& temp = managerPair.second->OnGetInitializeOperations();

			if (temp.size() == 0) continue;

			std::string managerName = managerPair.second->Name();
			for (auto& operationWrapper : temp)
			{
				initializers.emplace_back(std::pair<Utility::OperationWrapper, std::string>(std::move(operationWrapper), managerName));
			}
		}

		std::sort(initializers.begin(), initializers.end(), [](const auto& x, const auto& y)->bool {
			return x.first.layer == y.first.layer ? x.first.index < y.first.index : x.first.layer < y.first.layer;
		});

		for (auto& initializer : initializers)
		{
			initializer.first.operation();
			std::cout << "Layer: " << initializer.first.layer << "\tIndex: " << initializer.first.index << "\tManager: " << initializer.second << "\tDescription: " << initializer.first.description << std::endl;
		}
	}
	std::cout << "------------------------------" << std::endl;

	std::cout << "----------Post Initialize----------" << std::endl;
	{
		std::vector<std::pair<Utility::OperationWrapper, std::string>> postInitializers{};
		for (auto& managerPair : _managerTable)
		{
			auto&& temp = managerPair.second->OnGetPostInitializeOperation();
			if (!temp.operation) continue;
			postInitializers.emplace_back(std::pair<Utility::OperationWrapper, std::string>(std::move(temp), managerPair.second->Name()));
		}

		std::sort(postInitializers.begin(), postInitializers.end(), [](const auto& x, const auto& y)->bool {
			return x.first.layer == y.first.layer ? x.first.index < y.first.index : x.first.layer < y.first.layer;
		});

		for (auto& initializer : postInitializers)
		{
			initializer.first.operation();
			std::cout << "Layer: " << initializer.first.layer << "\tIndex: " << initializer.first.index << "\tManager: " << initializer.second << "\tDescription: " << initializer.first.description << std::endl;
		}
	}
	std::cout << "------------------------------" << std::endl;

	{
		std::shared_ptr<std::vector<Utility::Operation>> updateOperations = std::make_shared<std::vector<Utility::Operation>>();
		{
			std::cout << "----------Update----------" << std::endl;
			std::vector<std::pair<Utility::OperationWrapper, std::string>> updaters{};
			for (auto& managerPair : _managerTable)
			{
				auto&& temp = managerPair.second->OnGetUpdateOperations();

				if (temp.size() == 0) continue;

				std::string managerName = managerPair.second->Name();
				for (auto& operationWrapper : temp)
				{
					updaters.emplace_back(std::pair<Utility::OperationWrapper, std::string>(std::move(operationWrapper), managerName));
				}
			}

			std::sort(updaters.begin(), updaters.end(), [](const auto& x, const auto& y)->bool {
				return x.first.layer == y.first.layer ? x.first.index < y.first.index : x.first.layer < y.first.layer;
			});

			for (auto& updater : updaters)
			{
				updateOperations->emplace_back(std::move(updater.first.operation));
				std::cout << "Layer: " << updater.first.layer << "\tIndex: " << updater.first.index << "\tManager: " << updater.second << "\tDescription: " << updater.first.description << std::endl;
			}
			std::cout << "------------------------------" << std::endl;
		}

		tf::Taskflow mainLoopTaskFlow("MainLoopTaskFlow");
		auto&& mainLoopTask = mainLoopTaskFlow.emplace(
			[updateOperations]()->void
			{
				while (true)
				{
					for (const auto& updater : *updateOperations)
					{
						updater();
					}
					std::this_thread::yield();
				}
			}
		); 

		_mainLoopFuture = Manager::TaskManager::Executor().run(std::move(mainLoopTaskFlow));
	}
}

void AirEngine::Runtime::Core::Manager::MainManager::Finalize()
{
	std::cout << "----------Pre Finalize----------" << std::endl;
	{
		std::vector<std::pair<Utility::OperationWrapper, std::string>> preFinalizers{};
		for (auto& managerPair : _managerTable)
		{
			auto&& temp = managerPair.second->OnGetPreFinalizeOperation();
			if (!temp.operation) continue;
			preFinalizers.emplace_back(std::pair<Utility::OperationWrapper, std::string>(std::move(temp), managerPair.second->Name()));
		}

		std::sort(preFinalizers.begin(), preFinalizers.end(), [](const auto& x, const auto& y)->bool {
			return x.first.layer == y.first.layer ? x.first.index < y.first.index : x.first.layer < y.first.layer;
			});

		for (auto& finalizer : preFinalizers)
		{
			finalizer.first.operation();
			std::cout << "Layer: " << finalizer.first.layer << "\tIndex: " << finalizer.first.index << "\tManager: " << finalizer.second << "\tDescription: " << finalizer.first.description << std::endl;
		}
	}
	std::cout << "------------------------------" << std::endl;

	std::cout << "----------Finalize----------" << std::endl;
	{
		std::vector<std::pair<Utility::OperationWrapper, std::string>> finalizers{};
		for (auto& managerPair : _managerTable)
		{
			auto&& temp = managerPair.second->OnGetFinalizeOperations();

			if (temp.size() == 0) continue;

			std::string managerName = managerPair.second->Name();
			for (auto& operationWrapper : temp)
			{
				finalizers.emplace_back(std::pair<Utility::OperationWrapper, std::string>(std::move(operationWrapper), managerName));
			}
		}

		std::sort(finalizers.begin(), finalizers.end(), [](const auto& x, const auto& y)->bool {
			return x.first.layer == y.first.layer ? x.first.index < y.first.index : x.first.layer < y.first.layer;
			});

		for (auto& finalizer : finalizers)
		{
			finalizer.first.operation();
			std::cout << "Layer: " << finalizer.first.layer << "\tIndex: " << finalizer.first.index << "\tManager: " << finalizer.second << "\tDescription: " << finalizer.first.description << std::endl;
		}

	}
	std::cout << "------------------------------" << std::endl;

	std::cout << "----------Post Finalize----------" << std::endl;
	{
		std::vector<std::pair<Utility::OperationWrapper, std::string>> postFinalizers{};
		for (auto& managerPair : _managerTable)
		{
			auto&& temp = managerPair.second->OnGetPostFinalizeOperation();
			if (!temp.operation) continue;
			postFinalizers.emplace_back(std::pair<Utility::OperationWrapper, std::string>(std::move(temp), managerPair.second->Name()));
		}

		std::sort(postFinalizers.begin(), postFinalizers.end(), [](const auto& x, const auto& y)->bool {
			return x.first.layer == y.first.layer ? x.first.index < y.first.index : x.first.layer < y.first.layer;
			});

		for (auto& finalizer : postFinalizers)
		{
			finalizer.first.operation();
			std::cout << "Layer: " << finalizer.first.layer << "\tIndex: " << finalizer.first.index << "\tManager: " << finalizer.second << "\tDescription: " << finalizer.first.description << std::endl;
		}
	}
	std::cout << "------------------------------" << std::endl;
}

void AirEngine::Runtime::Core::Manager::MainManager::ResetManager(std::shared_ptr<Manager::ManagerBase> manager)
{
	_managerTable[manager->Name()] = manager;
}
