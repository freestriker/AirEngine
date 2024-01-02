#include "ThirdPartyLibraryManager.hpp"
#include <iostream>
#include "../Scene/Scene.hpp"
#include "../../Utility/InternedString.hpp"
#include "../../Utility/GarbageCollectInclude.hpp"
#include "../Scene/SceneObject.hpp"
#include <opencv2/core/utils/logger.hpp>
#include <opencv2/core/utility.hpp>

std::vector<AirEngine::Runtime::Utility::OperationWrapper> AirEngine::Runtime::Core::Manager::ThirdPartyLibraryManager::OnGetInternalInitializers()
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
		},
		{
			0, 0,
			[this]()->void
			{
				cv::utils::logging::setLogLevel(cv::utils::logging::LOG_LEVEL_ERROR);
				cv::setNumThreads(1);
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