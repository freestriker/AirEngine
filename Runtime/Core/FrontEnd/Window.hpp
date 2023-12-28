#pragma once
#include "../../Utility/ExportMacro.hpp"
#include "../../Utility/ContructorMacro.hpp"
#include <qwindow.h>
#include "FrontEndBase.hpp"
#include <qvulkaninstance.h>

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
					void OnCreate() override;
					void OnAcquireImage() override;
					void OnPresent() override;
					void OnSetVulkanHandle() override;
					void OnRecreateVulkanSwapchain() override;
					void OnDestroyVulkanSwapchain() override;
					QVulkanInstance _qVulkanInstance;
					std::vector<FrameResource> _frameResources;
					std::vector<ImageResource> _imageResources;
					uint32_t _curFrameIndex;
					uint32_t _curImageIndex;
					Graphic::Command::CommandPool* _commandPool;
					Graphic::Command::CommandBuffer* _commandBuffer;
					Graphic::Command::Fence* _transferFence;
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