#include <qdebug.h>
#include "GraphicDeviceManager.hpp"

std::vector<AirEngine::Runtime::Core::Manager::ManagerInitializerWrapper> AirEngine::Runtime::Core::Manager::GraphicDeviceManager::OnGetManagerInitializers() const
{
	return {
		{
			0, 0,
			[this]()->void {
				qDebug() << "Init" << QString(Name().c_str());
			}
		}
	};
}

void AirEngine::Runtime::Core::Manager::GraphicDeviceManager::OnFinishInitialize() const
{
	qDebug() << "Finish" << QString(Name().c_str());
}

AirEngine::Runtime::Core::Manager::GraphicDeviceManager::GraphicDeviceManager()
	: ManagerBase("GraphicMemoryManager")
{
}

AirEngine::Runtime::Core::Manager::GraphicDeviceManager::~GraphicDeviceManager()
{
}
