#include "ThirdPartyLibraryManager.hpp"
#include "AirEngine/Runtime/Utility/InternedString.hpp"
#include <opencv2/core/utils/logger.hpp>
#include <opencv2/core/utility.hpp>

AirEngine::Runtime::Utility::OperationWrapper AirEngine::Runtime::Core::Manager::ThirdPartyLibraryManager::OnGetPreInitializeOperation()
{
	return {
		0, 0,
		[]()->void
		{
			Utility::InternedString::Initialize();

			cv::utils::logging::setLogLevel(cv::utils::logging::LOG_LEVEL_ERROR);
			cv::setNumThreads(1);
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