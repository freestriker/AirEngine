#pragma once
#include "ManagerBase.hpp"

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
					static void RenderUpdate();
				protected:
					virtual std::vector<Utility::OperationWrapper> OnGetInitializeOperations() override;
					virtual std::vector<Utility::OperationWrapper> OnGetUpdateOperations() override;
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