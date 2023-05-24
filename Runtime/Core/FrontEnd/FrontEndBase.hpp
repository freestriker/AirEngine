#pragma once
#include "../../Utility/ExportMacro.hpp"
#include "../../Utility/ContructorMacro.hpp"
#include <vulkan/vulkan_core.h>
#include <VkBootstrap.h>

namespace AirEngine
{
	namespace Runtime
	{
		namespace Core
		{
			namespace Manager
			{
				class RenderManager;
			}
			namespace FrontEnd
			{
				class AIR_ENGINE_API FrontEndBase
				{
					friend class Manager::RenderManager;
				private:
					const bool _isWindow;
				protected:
					NO_COPY_MOVE(FrontEndBase)
					FrontEndBase();
					FrontEndBase(const bool isWindow);
					virtual ~FrontEndBase() = default;
					virtual void OnCreate() = 0;
					virtual void OnPreparePresent() = 0;
					virtual void OnPresent() = 0;
					virtual void OnFinishPresent() = 0;
				public:
					inline bool IsWindow() const
					{
						return _isWindow;
					}
				};
				class AIR_ENGINE_API WindowFrontEndBase
					: public FrontEndBase
				{
					friend class Manager::RenderManager;
				private:
				protected:
					vkb::Swapchain _vkbSwapchain;
					VkSwapchainKHR _vkSwapchain;
					VkSurfaceKHR _vkSurface;
					NO_COPY_MOVE(WindowFrontEndBase)
					WindowFrontEndBase();
					virtual ~WindowFrontEndBase() = default;
					virtual void OnSetVulkanHandle() = 0;
					virtual void OnCreateVulkanSwapchain() = 0;

				public:
					inline vkb::Swapchain VkbSwapchain() const
					{
						return _vkbSwapchain;
					}
					inline VkSwapchainKHR VkSwapchain() const
					{
						return _vkSwapchain;
					}
					inline VkSurfaceKHR VkSurface() const
					{
						return _vkSurface;
					}
				};
			}
		}
	}
}