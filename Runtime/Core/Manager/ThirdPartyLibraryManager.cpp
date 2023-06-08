#include "ThirdPartyLibraryManager.hpp"
#include <iostream>
#include "../Scene/Scene.hpp"
#include "../../Utility/InternedString.hpp"
#include "../../Utility/GarbageCollectInclude.hpp"
#include "../Scene/SceneObject.hpp"

std::vector<AirEngine::Runtime::Utility::InitializerWrapper> AirEngine::Runtime::Core::Manager::ThirdPartyLibraryManager::OnGetInternalInitializers()
{
	return
	{
		{
			0, 0,
			[this]()->void
			{
				Utility::InternedString::Initialize();
			}
		},
		{
			0, 0,
			[this]()->void
			{
				GC_INIT();
			}
		}
	};
}

AirEngine::Runtime::Core::Manager::ThirdPartyLibraryManager::ThirdPartyLibraryManager()
	: ManagerBase("ThirdPartyLibraryManager")
{
}

AirEngine::Runtime::Core::Manager::ThirdPartyLibraryManager::~ThirdPartyLibraryManager()
{
}