#pragma once
#include "ManagerBase.hpp"
#include <memory>
#include "../Boot/ManagerInitializer.hpp"
#include "qwindow.h"

namespace AirEngine
{
	namespace Runtime
	{
		namespace Core
		{
			namespace FrontEnd
			{
				class FrontEndBase;
			}
			namespace Manager
			{
				class AIR_ENGINE_API RenderManager
					: public ManagerBase
				{
				private:
					NO_COPY_MOVE(RenderManager)
					static void CreateMainWindow();
					static void CreateSwapchain();
				protected:
					virtual std::vector<Boot::ManagerInitializerWrapper> OnGetManagerInitializers() override;
					virtual void OnFinishInitialize() override;
				public:
					RenderManager();
					virtual ~RenderManager();
					static inline FrontEnd::FrontEndBase& FrontEnd()
					{
						return *_frontEnd;
					}
				protected:
					static FrontEnd::FrontEndBase* _frontEnd;
				public:
				};
			}
		}
	}
}