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

using namespace AirEngine::Runtime;

AirEngine::Runtime::Core::Manager::MainManager::MainManager()
	: Manager::ManagerBase("MainManager")
	, _managerTable{}
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
	{
		std::vector<Utility::InitializerWrapper> initializers{};
		for (auto& managerPair : _managerTable)
		{
			auto&& temp = managerPair.second->OnGetInitializer();
			initializers.emplace_back(std::move(temp));
		}

		std::sort(initializers.begin(), initializers.end(), [](const auto& x, const auto& y)->bool {
			return x.layer == y.layer ? x.index < y.index : x.layer < y.layer;
		});

		for (auto& initializer : initializers)
		{
			initializer.operation();
		}
	}

	{
		std::vector<Utility::InitializerWrapper> internaInitializers{};
		for (auto& managerPair : _managerTable)
		{
			auto&& temp = managerPair.second->OnGetInternalInitializers();
			internaInitializers.insert(internaInitializers.end(), temp.begin(), temp.end());
		}

		std::sort(internaInitializers.begin(), internaInitializers.end(), [](const auto& x, const auto& y)->bool {
			return x.layer == y.layer ? x.index < y.index : x.layer < y.layer;
		});

		for (auto& initializer : internaInitializers)
		{
			initializer.operation();
		}
	}
}

void AirEngine::Runtime::Core::Manager::MainManager::Finalize()
{
	{
		std::vector<Utility::InitializerWrapper> internaFinalizers{};
		for (auto& managerPair : _managerTable)
		{
			auto&& temp = managerPair.second->OnGetInternalFinalizers();
			internaFinalizers.insert(internaFinalizers.end(), temp.begin(), temp.end());
		}

		std::sort(internaFinalizers.begin(), internaFinalizers.end(), [](const auto& x, const auto& y)->bool {
			return x.layer == y.layer ? x.index < y.index : x.layer < y.layer;
		});

		for (auto& finalizer : internaFinalizers)
		{
			finalizer.operation();
		}
	}

	{
		std::vector<Utility::InitializerWrapper> finalizers{};
		for (auto& managerPair : _managerTable)
		{
			auto&& temp = managerPair.second->OnGetFinalizer();
			finalizers.emplace_back(std::move(temp));
		}

		std::sort(finalizers.begin(), finalizers.end(), [](const auto& x, const auto& y)->bool {
			return x.layer == y.layer ? x.index < y.index : x.layer < y.layer;
			});

		for (auto& finalizer : finalizers)
		{
			finalizer.operation();
		}
	}
}

void AirEngine::Runtime::Core::Manager::MainManager::ResetManager(std::shared_ptr<Manager::ManagerBase> manager)
{
	_managerTable[manager->Name()] = manager;
}
