#pragma once
#include "AirEngine/Runtime/Utility/ExportMacro.hpp"
#include "AirEngine/Runtime/Utility/ContructorMacro.hpp"
#include <vulkan/vulkan.hpp>
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
				public:
				private:
					const bool _isWindow;
				protected:
					NO_COPY_MOVE(FrontEndBase)
					FrontEndBase();
					FrontEndBase(const bool isWindow);
					virtual ~FrontEndBase() = default;
					virtual void OnCreate() = 0;
					virtual void OnAcquireImage() = 0;
					virtual void OnPresent() = 0;
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
					vk::SwapchainKHR _vkSwapchain;
					vk::SurfaceKHR _vkSurface;
					NO_COPY_MOVE(WindowFrontEndBase)
					WindowFrontEndBase();
					virtual ~WindowFrontEndBase() = default;
					virtual void OnSetVulkanHandle() = 0;
					virtual void OnRecreateVulkanSwapchain() = 0;
					virtual void OnDestroyVulkanSwapchain() = 0;

				public:
					inline vkb::Swapchain VkbSwapchain() const
					{
						return _vkbSwapchain;
					}
					inline vk::SwapchainKHR VkSwapchain() const
					{
						return _vkSwapchain;
					}
					inline vk::SurfaceKHR VkSurface() const
					{
						return _vkSurface;
					}
				};
			}
		}
	}
}