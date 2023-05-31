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
					struct Frame {
						Graphic::Command::Fence* fence = nullptr;
						bool fenceWaitable = false;
						Graphic::Command::Semaphore* imageSemaphore = nullptr;
						Graphic::Command::Semaphore* drawSemaphore = nullptr;
						Graphic::Command::Semaphore* presTransSemaphore = nullptr;
						bool imageAcquired = false;
						bool imageSemWaitable = false;
					};

				private:
					void OnCreate() override;
					void OnPreparePresent() override;
					void OnPresent() override;
					void OnFinishPresent() override;
					void OnSetVulkanHandle() override;
					void OnCreateVulkanSwapchain() override;
					QVulkanInstance _qVulkanInstance;
					std::vector<Frame> _frames;
					std::vector<Graphic::Instance::Image*> _swapchainImages;
					uint32_t _swapchainCurrentFrameIndex;
					Graphic::Command::CommandPool* _commandPool;
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