#pragma once
#include "ManagerBase.hpp"
#include <memory>
#include "../../Utility/Initializer.hpp"
#include "../../Utility/Fiber.hpp"

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
					static void AddRenderLoop();
					static void RenderLoop();
					static Utility::Fiber::fiber _renderLoopFiber;
				protected:
					virtual std::vector<Utility::InitializerWrapper> OnGetManagerInitializers() override;
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