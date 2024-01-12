#pragma once
#include "AirEngine/Runtime/Utility/ExportMacro.hpp"
#include "AirEngine/Runtime/Utility/ContructorMacro.hpp"
#include <qwindow.h>
#include "FrontEndBase.hpp"
#include <qvulkaninstance.h>
#include "AirEngine/Runtime/Utility/Condition.hpp"
#include <mutex>
#include <condition_variable>
#include "AirEngine/Runtime/Graphic/Instance/RenderPassBase.hpp"
#include <memory>

namespace AirEngine
{
	namespace Runtime
	{
		namespace Graphic
		{
			namespace Instance
			{
				class Image;
				class FrameBuffer;
			}
			namespace Command
			{
				class Semaphore;
				class Fence;
				class CommandPool;
				class CommandBuffer;
			}
		}
		namespace FrontEnd
		{
			class PresentRenderPass final
				: public AirEngine::Runtime::Graphic::Instance::RenderPassBase
			{
				REFLECTABLE_OBJECT
			public:
				INVOKABLE PresentRenderPass();
				INVOKABLE ~PresentRenderPass() = default;
				NO_COPY_MOVE(PresentRenderPass);
			};
			class AIR_ENGINE_API Window
				: public QWindow
				, public WindowFrontEndBase
			{
			private:
				struct FrameResource {
					std::unique_ptr<Graphic::Command::Fence> acquireFence;
					std::unique_ptr<Graphic::Command::Semaphore> acquireSemaphore;
				};
				struct ImageResource {
					std::unique_ptr<Graphic::Instance::Image> image;
					std::unique_ptr<Graphic::Instance::FrameBuffer> frameBuffer;
					std::unique_ptr<Graphic::Command::Semaphore> transferSemaphore;
				};

			private:
				void LoadPresentData();
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
				std::unique_ptr<Graphic::Command::CommandPool> _commandPool;
				std::unique_ptr<Graphic::Command::Fence> _transferFence;
				std::unique_ptr<PresentRenderPass> _presentRenderPass;
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