#include "ThirdPartyLibraryManager.hpp"
#include <iostream>
#include "../Scene/Scene.hpp"
#include "../../Utility/InternedString.hpp"

std::vector<AirEngine::Runtime::Core::Boot::ManagerInitializerWrapper> AirEngine::Runtime::Core::Manager::ThirdPartyLibraryManager::OnGetManagerInitializers()
{
	return
	{
		{
			0, 0,
			[this]()->void
			{
				Utility::InternedString::Initialize();
			}
		}
	};
}

void AirEngine::Runtime::Core::Manager::ThirdPartyLibraryManager::OnFinishInitialize()
{
	std::cout << "Finish initialize " << Name() << std::endl;
}

AirEngine::Runtime::Core::Manager::ThirdPartyLibraryManager::ThirdPartyLibraryManager()
	: ManagerBase("ThirdPartyLibraryManager")
{
}

AirEngine::Runtime::Core::Manager::ThirdPartyLibraryManager::~ThirdPartyLibraryManager()
{
}