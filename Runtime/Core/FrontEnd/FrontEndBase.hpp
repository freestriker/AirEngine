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
					bool _isReadyToRender;
				protected:
					NO_COPY_MOVE(FrontEndBase)
					FrontEndBase();
					FrontEndBase(const bool isWindow);
					virtual ~FrontEndBase() = default;
					virtual void OnCreate() = 0;
					virtual void OnFinishRender()
					{

					}
					virtual void OnStartRender()
					{

					}
				public:
					inline bool IsWindow() const
					{
						return _isWindow;
					}
					inline bool IsReadyToRender() const
					{
						return _isReadyToRender;
					}
					void ReadyToRender()
					{
						_isReadyToRender = true;
					}
					void StartRender()
					{
						OnStartRender();
					}
					void FinishRender()
					{
						_isReadyToRender = false;
						OnFinishRender();
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
					virtual void OnCreateVulkanSwapchain() = 0;

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