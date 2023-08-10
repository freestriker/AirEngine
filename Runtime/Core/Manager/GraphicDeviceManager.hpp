#pragma once
#include "ManagerBase.hpp"
#include "../../Utility/Initializer.hpp"
#include <VkBootstrap.h>
#include <vk_mem_alloc.h>
#include <unordered_map>
#include "../../Utility/InternedString.hpp"
#include "../../Graphic/Instance/Queue.hpp"
#include "vulkan/vulkan.hpp"

namespace AirEngine
{
	namespace Runtime
	{
		namespace Core
		{
			namespace Manager
			{
				class AIR_ENGINE_API GraphicDeviceManager
					: public ManagerBase
				{
				private:
					NO_COPY_MOVE(GraphicDeviceManager)
				protected:
					static vkb::Instance _vkbInstance;
					static vkb::PhysicalDevice _vkbPhysicalDevice;
					static vkb::Device _vkbDevice;

					static vk::Instance _instance;
					static vk::PhysicalDevice _physicalDevice;
					static vk::Device _device;

					static VmaAllocator _vmaAllocator;

					static std::unordered_map<Utility::InternedString, std::unique_ptr<Graphic::Instance::Queue>> _queueMap;

					virtual std::vector<Utility::InitializerWrapper> OnGetInternalInitializers() override;
					static void CreateVulkanInstance();
					static void CreateDevice();
					static void SetDefaultDispatcher();
					static void PopulateQueue();
					static void CreateMemoryAllocator();
					static void InitializeGraphicManagers();
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

					GraphicDeviceManager();
					virtual ~GraphicDeviceManager();
				};
			}
		}
	}
}