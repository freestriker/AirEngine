#include "GraphicDeviceManager.hpp"
#include <iostream>
#include <mutex>
#include <vk_mem_alloc.h>
#include "RenderManager.hpp"
#include "../FrontEnd/FrontEndBase.hpp"

VkInstance AirEngine::Runtime::Core::Manager::GraphicDeviceManager::_vkInstance = VK_NULL_HANDLE;
VkPhysicalDevice AirEngine::Runtime::Core::Manager::GraphicDeviceManager::_vkPhysicalDevice{ VK_NULL_HANDLE };
VkDevice AirEngine::Runtime::Core::Manager::GraphicDeviceManager::_vkDevice{ VK_NULL_HANDLE };

QVulkanInstance AirEngine::Runtime::Core::Manager::GraphicDeviceManager::_qVulkanInstance{};

vkb::Instance AirEngine::Runtime::Core::Manager::GraphicDeviceManager::_vkbInstance{};
vkb::PhysicalDevice AirEngine::Runtime::Core::Manager::GraphicDeviceManager::_vkbPhysicalDevice{};
vkb::Device AirEngine::Runtime::Core::Manager::GraphicDeviceManager::_vkbDevice{};

VmaAllocator AirEngine::Runtime::Core::Manager::GraphicDeviceManager::_vmaAllocator{ VK_NULL_HANDLE };

std::unordered_map<AirEngine::Runtime::Utility::InternedString, std::unique_ptr<AirEngine::Runtime::Graphic::Instance::Queue>> AirEngine::Runtime::Core::Manager::GraphicDeviceManager::_queueMap{ };

static VKAPI_ATTR VkBool32 VKAPI_CALL DebugCallback(VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity, VkDebugUtilsMessageTypeFlagsEXT messageType, const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData, void* pUserData)
{
	std::string s = pCallbackData->pMessage;
	s = "Exception: " + s + "\n";
	std::cout << s;
	return VK_FALSE;
}

std::vector<AirEngine::Runtime::Utility::InitializerWrapper> AirEngine::Runtime::Core::Manager::GraphicDeviceManager::OnGetManagerInitializers()
{
	return {
		{ 0, 0, CreateVulkanInstance },
		{ 0, 2, CreateDevice },
		{ 0, 4, CreateMemoryAllocator },
	};
}

void AirEngine::Runtime::Core::Manager::GraphicDeviceManager::OnFinishInitialize()
{
	std::cout << "Finish initialize " << Name() << std::endl;
}

void AirEngine::Runtime::Core::Manager::GraphicDeviceManager::CreateVulkanInstance()
{
	_vkInstance = VK_NULL_HANDLE;
	vkb::InstanceBuilder instanceBuilder;
	auto vkbResult = instanceBuilder
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
	if (!vkbResult) qFatal("Create vulkan instance failed.");
	_vkbInstance = vkbResult.value();
	_vkInstance = _vkbInstance.instance;
	_qVulkanInstance.setVkInstance(_vkInstance);
	bool qResult = _qVulkanInstance.create();
	if (!qResult) qFatal("Create vulkan instance failed.");
}

void AirEngine::Runtime::Core::Manager::GraphicDeviceManager::CreateDevice()
{
	const bool isWindow{ RenderManager::FrontEnd().IsWindow() };

	VkPhysicalDeviceShaderAtomicFloatFeaturesEXT vkPhysicalDeviceShaderAtomicFloatFeaturesEXT{};
	vkPhysicalDeviceShaderAtomicFloatFeaturesEXT.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_SHADER_ATOMIC_FLOAT_FEATURES_EXT;
	vkPhysicalDeviceShaderAtomicFloatFeaturesEXT.shaderSharedFloat32Atomics = VK_TRUE;

	vkb::PhysicalDeviceSelector physicalDeviceSelector(_vkbInstance);
	physicalDeviceSelector = physicalDeviceSelector
		.add_required_extension(VK_EXT_SHADER_ATOMIC_FLOAT_EXTENSION_NAME)
		.add_required_extension_features(vkPhysicalDeviceShaderAtomicFloatFeaturesEXT);
	if (isWindow)
	{
		physicalDeviceSelector.set_surface(dynamic_cast<FrontEnd::WindowFrontEndBase&>(RenderManager::FrontEnd()).VkSurface());
		physicalDeviceSelector.require_present(true);
	}
	else
	{
		physicalDeviceSelector.require_present(false);
	}
	auto physicalDeviceResult = physicalDeviceSelector.select();
	if (!physicalDeviceResult) 
	{
		qFatal((std::string("Failed to select Vulkan Physical Device. Error: ") + std::string(physicalDeviceResult.error().message())).c_str());
	}
	_vkbPhysicalDevice = physicalDeviceResult.value();
	_vkPhysicalDevice = _vkbPhysicalDevice.physical_device;

	std::vector<vkb::CustomQueueDescription> customQueueDescriptions;
	uint32_t graphicQueueFamily = 0;
	uint32_t transferQueueFamily = 0;
	uint32_t presentQueueFamily = 0;
	auto queue_families = _vkbPhysicalDevice.get_queue_families();
	for (uint32_t i = 0; i < (uint32_t)queue_families.size(); i++) 
	{
		if (queue_families[i].queueFlags & VK_QUEUE_GRAPHICS_BIT) 
		{
			if (isWindow)
			{
				customQueueDescriptions.push_back(vkb::CustomQueueDescription(i, 1, { 1.0f }));
				presentQueueFamily = i;
			}
			else
			{
				customQueueDescriptions.push_back(vkb::CustomQueueDescription(i, 2, {1.0f, 1.0f}));
				graphicQueueFamily = i;
				presentQueueFamily = i;
			}
			break;
		}
	}
	if (_vkbPhysicalDevice.has_dedicated_transfer_queue()) 
	{
		for (uint32_t i = 0; i < (uint32_t)queue_families.size(); i++) 
		{
			if ((queue_families[i].queueFlags & VK_QUEUE_TRANSFER_BIT) &&
				(queue_families[i].queueFlags & VK_QUEUE_GRAPHICS_BIT) == 0 &&
				(queue_families[i].queueFlags & VK_QUEUE_COMPUTE_BIT) == 0)
			{
				customQueueDescriptions.push_back(vkb::CustomQueueDescription(i, 1, { 1.0f }));
				transferQueueFamily = i;
				break;
			}
		}
	}
	else if (_vkbPhysicalDevice.has_separate_transfer_queue()) 
	{
		for (uint32_t i = 0; i < (uint32_t)queue_families.size(); i++) 
		{
			if ((queue_families[i].queueFlags & VK_QUEUE_TRANSFER_BIT) &&
				((queue_families[i].queueFlags & VK_QUEUE_GRAPHICS_BIT) == 0))
			{
				customQueueDescriptions.push_back(vkb::CustomQueueDescription(i, 1, { 1.0f }));
				transferQueueFamily = i;
				break;
			}
		}
	}

	vkb::DeviceBuilder deviceBuilder(_vkbPhysicalDevice);
	auto deviceResult = deviceBuilder
		.custom_queue_setup(customQueueDescriptions)
		.build();
	if (!deviceResult) 
	{
		qFatal((std::string("Failed to create Vulkan device. Error: ") + std::string(deviceResult.error().message())).c_str());
	}
	_vkbDevice = deviceResult.value();
	_vkDevice = _vkbDevice.device;

	_queueMap.insert(std::make_pair(Utility::InternedString("GraphicQueue"), new Graphic::Instance::Queue(_vkbDevice.get_queue(vkb::QueueType::graphics).value(), graphicQueueFamily, Utility::InternedString("GraphicQueue"))));
	_queueMap.insert(std::make_pair(Utility::InternedString("TransferQueue"), new Graphic::Instance::Queue(_vkbDevice.get_queue(vkb::QueueType::transfer).value(), transferQueueFamily, Utility::InternedString("TransferQueue"))));
	if (isWindow)
	{
		_queueMap.insert(std::make_pair(Utility::InternedString("PresentQueue"), new Graphic::Instance::Queue(_vkbDevice.get_queue(vkb::QueueType::present).value(), presentQueueFamily, Utility::InternedString("PresentQueue"))));
	}
}

void AirEngine::Runtime::Core::Manager::GraphicDeviceManager::CreateMemoryAllocator()
{
	VmaVulkanFunctions vulkanFunctions = {};
	vulkanFunctions.vkGetInstanceProcAddr = &vkGetInstanceProcAddr;
	vulkanFunctions.vkGetDeviceProcAddr = &vkGetDeviceProcAddr;

	VmaAllocatorCreateInfo allocatorCreateInfo{};
	//allocatorCreateInfo.vulkanApiVersion = VK_VERSION_1_3;
	allocatorCreateInfo.physicalDevice = _vkPhysicalDevice;
	allocatorCreateInfo.device = _vkDevice;
	allocatorCreateInfo.instance = _vkInstance;
	allocatorCreateInfo.pVulkanFunctions = &vulkanFunctions;

	vmaCreateAllocator(&allocatorCreateInfo, &_vmaAllocator);
}

AirEngine::Runtime::Core::Manager::GraphicDeviceManager::GraphicDeviceManager()
	: ManagerBase("GraphicDeviceManager")
{
}

AirEngine::Runtime::Core::Manager::GraphicDeviceManager::~GraphicDeviceManager()
{
}
