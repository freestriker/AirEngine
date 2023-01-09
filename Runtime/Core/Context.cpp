#include "Context.hpp"
#include "Manager/GraphicDeviceManager.hpp"
#include "Manager/ManagerInitializer.hpp"
#include <algorithm>

using namespace AirEngine::Runtime;

Core::Manager::GraphicDeviceManager* const Core::Context::_graphicDeviceManager = new Core::Manager::GraphicDeviceManager();

std::map<std::string, AirEngine::Runtime::Core::Manager::ManagerBase*> const AirEngine::Runtime::Core::Context::_managerTable{
	{"GraphicDeviceManager", Core::Context::_graphicDeviceManager}
};

void Core::Context::Init()
{
	std::vector<Manager::ManagerInitializerWrapper> initializers{};
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
		initializer.operation();
	}

	for (auto& managerPair : _managerTable)
	{
		managerPair.second->OnFinishInitialize();
	}
}
Core::Manager::GraphicDeviceManager& Core::Context::GraphicDeviceManager()
{
	return *_graphicDeviceManager;
}