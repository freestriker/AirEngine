#include "ThirdPartyLibraryManager.hpp"
#include <iostream>
#include "../Scene/Scene.hpp"
#include "../../Utility/InternedString.hpp"
#include "../../Utility/GarbageCollectInclude.hpp"
#include "../Scene/SceneObject.hpp"

std::vector<AirEngine::Runtime::Utility::InitializerWrapper> AirEngine::Runtime::Core::Manager::ThirdPartyLibraryManager::OnGetManagerInitializers()
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
				auto&& cso = NEW_COLLECTABLE_OBJECT Scene::SceneObject();
				auto&& tso = NEW_TRACKABLE_OBJECT Scene::SceneObject();
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