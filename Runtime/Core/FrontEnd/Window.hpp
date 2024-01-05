#pragma once
#include "AirEngine/Runtime/Utility/ExportMacro.hpp"
#include "AirEngine/Runtime/Utility/ContructorMacro.hpp"
#include <qwindow.h>
#include "FrontEndBase.hpp"
#include <qvulkaninstance.h>
#include "AirEngine/Runtime/Utility/Condition.hpp"
#include <mutex>
#include <condition_variable>

namespace AirEngine
{
	namespace Runtime
	{
		namespace Graphic
		{
			namespace Instance
			{
				class Image;
			}
			namespace Command
			{
				class Semaphore;
				class Fence;
				class CommandPool;
				class CommandBuffer;
			}
		}
		namespace Core
		{
			namespace FrontEnd
			{
				class AIR_ENGINE_API Window
					: public QWindow
					, public WindowFrontEndBase
				{
				private:
					struct FrameResource {
						Graphic::Command::Fence* acquireFence = nullptr;
						Graphic::Command::Semaphore* acquireSemaphore = nullptr;
					};
					struct ImageResource {
						Graphic::Instance::Image* image = nullptr;
						Graphic::Command::Semaphore* transferSemaphore = nullptr;
					};

				private:
					void OnCreateSurface() override;
					void OnCreateSwapchain() override;
					bool AcquireImage();
					bool Present();
					void RecreateVulkanSwapchain();
					void DestroyVulkanSwapchain();
					Utility::Condition<std::mutex, std::condition_variable> _beginPresentCondition;
					Utility::Condition<std::mutex, std::condition_variable> _endPresentCondition;
					QVulkanInstance _qVulkanInstance;
					std::vector<FrameResource> _frameResources;
					std::vector<ImageResource> _imageResources;
					uint32_t _curFrameIndex;
					uint32_t _curImageIndex;
					Graphic::Command::CommandPool* _commandPool;
					Graphic::Command::CommandBuffer* _commandBuffer;
					Graphic::Command::Fence* _transferFence;
					void OnFinishRender()override;
					void OnStartRender()override;
				protected:
					void exposeEvent(QExposeEvent*) override;
					void resizeEvent(QResizeEvent*) override;
					bool event(QEvent*) override;
				public:
					NO_COPY_MOVE(Window)
					Window();
					~Window() = default;
					inline QVulkanInstance& QVulkanInstance()
					{
						return _qVulkanInstance;
					}
				};
			}
		}
	}
}