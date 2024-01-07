#pragma once
#include "ManagerBase.hpp"
#include <VkBootstrap.h>
#include <vk_mem_alloc.h>
#include <unordered_map>
#include "AirEngine/Runtime/Utility/InternedString.hpp"
#include "AirEngine/Runtime/Graphic/Instance/Queue.hpp"
#include "vulkan/vulkan.hpp"

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
				protected:
					static vkb::Instance _vkbInstance;
					static vkb::PhysicalDevice _vkbPhysicalDevice;
					static vkb::Device _vkbDevice;

					static vk::Instance _instance;
					static vk::PhysicalDevice _physicalDevice;
					static vk::Device _device;

					static VmaAllocator _vmaAllocator;

					static std::unordered_map<Utility::InternedString, std::unique_ptr<Graphic::Instance::Queue>> _queueMap;
					
					static FrontEnd::FrontEndBase* _frontEnd;

					virtual std::vector<Utility::OperationWrapper> OnGetInitializeOperations() override;
					static void CreateVulkanInstance();
					static void CreateSurface();
					static void CreateDevice();
					static void SetDefaultDispatcher();
					static void PopulateQueue();
					static void CreateSwapchain();
					static void CreateMemoryAllocator();

					virtual std::vector<Utility::OperationWrapper> OnGetUpdateOperations() override;
					static void RenderUpdate();
				public:
					static inline vkb::Instance& VkbInstance()
					{
						return _vkbInstance;
					}
					static inline vkb::PhysicalDevice& VkbPhysicalDevice()
					{
						return _vkbPhysicalDevice;
					}
					static inline vkb::Device& VkbDevice()
					{
						return _vkbDevice;
					}

					static inline vk::Instance& Instance()
					{
						return _instance;
					}
					static inline vk::PhysicalDevice& PhysicalDevice()
					{
						return _physicalDevice;
					}
					static inline vk::Device& Device()
					{
						return _device;
					}
					static inline VmaAllocator VmaAllocator()
					{
						return _vmaAllocator;
					}

					static inline Graphic::Instance::Queue& Queue(Utility::InternedString queueName)
					{
						return *_queueMap[queueName];
					}

					static inline FrontEnd::FrontEndBase& FrontEnd()
					{
						return *_frontEnd;
					}

					RenderManager();
					virtual ~RenderManager();
				};
			}
		}
	}
}