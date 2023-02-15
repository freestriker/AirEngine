#include "GraphicDeviceManager.hpp"
#include <VkBootstrap.h>
#include <iostream>
#include <mutex>

VkInstance AirEngine::Runtime::Core::Manager::GraphicDeviceManager::_vkInstance = VK_NULL_HANDLE;

static VKAPI_ATTR VkBool32 VKAPI_CALL DebugCallback(VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity, VkDebugUtilsMessageTypeFlagsEXT messageType, const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData, void* pUserData)
{
	static std::mutex mutex = {};
	std::unique_lock<std::mutex> lock(mutex);
	std::cout << pCallbackData->pMessage << std::endl;
	return VK_FALSE;
}

std::vector<AirEngine::Runtime::Core::Boot::ManagerInitializerWrapper> AirEngine::Runtime::Core::Manager::GraphicDeviceManager::OnGetManagerInitializers()
{
	return {
		{
			0, 0,
			[this]()->void {
				std::cout << "Initialize " << Name() << "at layer: " << 0 << ", index: " << 0 << std::endl;
				_vkInstance = VK_NULL_HANDLE;
				vkb::InstanceBuilder instance_builder;
				auto instance_builder_return = instance_builder
					.set_engine_name("AiEngine")
					.set_engine_version(VK_VERSION_1_3)
					.set_app_name("AiEngine")
					.set_app_version(VK_VERSION_1_3)
					.enable_validation_layers()

					.set_debug_messenger_type(
						//VkDebugUtilsMessageTypeFlagBitsEXT::VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT |
						VkDebugUtilsMessageTypeFlagBitsEXT::VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT |
						VkDebugUtilsMessageTypeFlagBitsEXT::VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT
					)
					.set_debug_messenger_severity(
						//VkDebugUtilsMessageSeverityFlagBitsEXT::VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT |
						//VkDebugUtilsMessageSeverityFlagBitsEXT::VK_DEBUG_UTILS_MESSAGE_SEVERITY_INFO_BIT_EXT |
						VkDebugUtilsMessageSeverityFlagBitsEXT::VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT |
						VkDebugUtilsMessageSeverityFlagBitsEXT::VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT
					)
					.set_debug_callback(DebugCallback)
					.build();
				if (!instance_builder_return) {
					throw std::runtime_error("Create vulkan instance failed.");
				}
				_vkInstance = instance_builder_return.value().instance;
			}
		}
	};
}

void AirEngine::Runtime::Core::Manager::GraphicDeviceManager::OnFinishInitialize()
{
	std::cout << "Finish initialize " << Name() << std::endl;
}

AirEngine::Runtime::Core::Manager::GraphicDeviceManager::GraphicDeviceManager()
	: ManagerBase("GraphicDeviceManager")
{
}

AirEngine::Runtime::Core::Manager::GraphicDeviceManager::~GraphicDeviceManager()
{
}
