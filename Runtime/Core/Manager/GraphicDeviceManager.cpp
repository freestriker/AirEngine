#include <qdebug.h>
#include "GraphicDeviceManager.hpp"

std::vector<AirEngine::Runtime::Core::Manager::ManagerInitializerWrapper> AirEngine::Runtime::Core::Manager::GraphicDeviceManager::OnGetManagerInitializers()
{
	return {
		{
			0, 0,
			[this]()->void {
				qDebug() << "Initialize" << Name().c_str() << "at layer: " << 0 << ", index: " << 0;
			}
		}
	};
}

void AirEngine::Runtime::Core::Manager::GraphicDeviceManager::OnFinishInitialize()
{
	qDebug() << "Finish initialize" << QString(Name().c_str());
}

AirEngine::Runtime::Core::Manager::GraphicDeviceManager::GraphicDeviceManager()
	: ManagerBase("GraphicMemoryManager")
{
}

AirEngine::Runtime::Core::Manager::GraphicDeviceManager::~GraphicDeviceManager()
{
}
