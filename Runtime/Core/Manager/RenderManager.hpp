#pragma once
#include "ManagerBase.hpp"
#include "AirEngine/Runtime/Utility/Condition.hpp"
#include <mutex>
#include <condition_variable>

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
					//enum class Status
					//{
					//	ACQUIRE,
					//	READY,
					//	RENDERING,
					//	PRESENT,
					//	NONE
					//};
					NO_COPY_MOVE(RenderManager)
					static void CreateMainWindow();
					static void CreateSwapchain();
					static void RenderUpdate();
					//static Status _status;
					//static Utility::Condition<std::mutex, std::condition_variable> _beginRenderCondition;
					//static Utility::Condition<std::mutex, std::condition_variable> _endPresentCondition;
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
					//static bool TryBeginRender();
					//static void EndRender();
				protected:
					static FrontEnd::FrontEndBase* _frontEnd;
				public:
				};
			}
		}
	}
}