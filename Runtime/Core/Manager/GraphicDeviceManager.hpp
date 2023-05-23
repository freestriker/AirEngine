#pragma once
#include "ManagerBase.hpp"
#include <vulkan/vulkan_core.h>
#include "../../Utility/Initializer.hpp"
#include <QVulkanInstance>
#include <qvulkaninstance.h>
#include <VkBootstrap.h>
#include <vk_mem_alloc.h>
#include <unordered_map>
#include "../../Utility/InternedString.hpp"
#include "../../Graphic/Instance/Queue.hpp"

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
					static VkInstance _vkInstance;
					static VkPhysicalDevice _vkPhysicalDevice;
					static VkDevice _vkDevice;

					static QVulkanInstance _qVulkanInstance;

					static vkb::Instance _vkbInstance;
					static vkb::PhysicalDevice _vkbPhysicalDevice;
					static vkb::Device _vkbDevice;

					static VmaAllocator _vmaAllocator;

					static std::unordered_map<Utility::InternedString, std::unique_ptr<Graphic::Instance::Queue>> _queueMap;

					virtual std::vector<Utility::InitializerWrapper> OnGetManagerInitializers() override;
					virtual void OnFinishInitialize() override;
					static void CreateVulkanInstance();
					static void CreateDevice();
					static void CreateMemoryAllocator();
				public:
					static inline VkInstance VkInstance()
					{
						return _vkInstance;
					}
					static inline VkPhysicalDevice VkPhysicalDevice()
					{
						return _vkPhysicalDevice;
					}
					static inline VkDevice VkDevice()
					{
						return _vkDevice;
					}
					static inline QVulkanInstance& QVulkanInstance()
					{
						return _qVulkanInstance;
					}
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