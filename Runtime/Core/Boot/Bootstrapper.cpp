#include "Bootstrapper.hpp"
#include <algorithm>
#include "../Manager/GraphicDeviceManager.hpp"
#include "../Manager/SceneManager.hpp"
#include "../Manager/ThirdPartyLibraryManager.hpp"

using namespace AirEngine::Runtime;

AirEngine::Runtime::Core::Boot::Bootstrapper::Bootstrapper()
	:_managerTable{}
{
	ResetManager(std::make_shared<Manager::ThirdPartyLibraryManager>());
	ResetManager(std::make_shared<Manager::SceneManager>());
	ResetManager(std::make_shared<Manager::GraphicDeviceManager>());
}

AirEngine::Runtime::Core::Boot::Bootstrapper::~Bootstrapper()
{
}

void Core::Boot::Bootstrapper::Boot()
{
	std::vector<Boot::ManagerInitializerWrapper> initializers{};
	for (auto& managerPair : _managerTable)
	{
		auto temp = managerPair.second->OnGetManagerInitializers();
		initializers.insert(initializers.end(), temp.begin(), temp.end());
	}

	std::sort(initializers.begin(), initializers.end(), [](const auto& x, const auto& y)->bool {
		return x.initLayer == y.initLayer ? x.initIndex < y.initIndex : x.initLayer < y.initLayer;
	});

	for (auto& initializer : initializers)
	{
		initializer.initializer();
	}

	for (auto& managerPair : _managerTable)
	{
		managerPair.second->OnFinishInitialize();
	}
}

void AirEngine::Runtime::Core::Boot::Bootstrapper::ResetManager(std::shared_ptr<Manager::ManagerBase> manager)
{
	_managerTable[manager->Name()] = manager;
}
