#include "GraphicDeviceManager.hpp"
#include <iostream>
#include <vk_mem_alloc.h>
#include "RenderManager.hpp"
#include "../FrontEnd/FrontEndBase.hpp"
#include <vulkan/vk_enum_string_helper.h>

VkInstance AirEngine::Runtime::Core::Manager::GraphicDeviceManager::_vkInstance = VK_NULL_HANDLE;
VkPhysicalDevice AirEngine::Runtime::Core::Manager::GraphicDeviceManager::_vkPhysicalDevice{ VK_NULL_HANDLE };
VkDevice AirEngine::Runtime::Core::Manager::GraphicDeviceManager::_vkDevice{ VK_NULL_HANDLE };

vkb::Instance AirEngine::Runtime::Core::Manager::GraphicDeviceManager::_vkbInstance{};
vkb::PhysicalDevice AirEngine::Runtime::Core::Manager::GraphicDeviceManager::_vkbPhysicalDevice{};
vkb::Device AirEngine::Runtime::Core::Manager::GraphicDeviceManager::_vkbDevice{};

VmaAllocator AirEngine::Runtime::Core::Manager::GraphicDeviceManager::_vmaAllocator{ VK_NULL_HANDLE };

std::unordered_map<AirEngine::Runtime::Utility::InternedString, std::unique_ptr<AirEngine::Runtime::Graphic::Instance::Queue>> AirEngine::Runtime::Core::Manager::GraphicDeviceManager::_queueMap{ };

std::unordered_map<std::string, VkFormat> AirEngine::Runtime::Core::Manager::GraphicDeviceManager::_vkFormatStringToEnumMap{ };
std::unordered_map<std::string, VkImageUsageFlagBits> AirEngine::Runtime::Core::Manager::GraphicDeviceManager::_vkImageUsageFlagBitsStringToEnumMap{ };
std::unordered_map<std::string, VkMemoryPropertyFlagBits> AirEngine::Runtime::Core::Manager::GraphicDeviceManager::_vkMemoryPropertyFlagBitsStringToEnumMap{ };

#ifndef NDEBUG
static VKAPI_ATTR VkBool32 VKAPI_CALL DebugCallback(VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity, VkDebugUtilsMessageTypeFlagsEXT messageType, const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData, void* pUserData)
{
	std::string s = pCallbackData->pMessage;
	s = "Exception: " + s + "\n";
	std::cout << s;
	return VK_FALSE;
}
#endif // !NDEBUG

std::vector<AirEngine::Runtime::Utility::InitializerWrapper> AirEngine::Runtime::Core::Manager::GraphicDeviceManager::OnGetInternalInitializers()
{
	return {
		{ 0, 0, CreateVulkanInstance },
		{ 0, 2, CreateDevice },
		{ 0, 4, CreateMemoryAllocator },
		{ 0, 5, PopulateVulkanStringToEnumMap }
	};
}

void AirEngine::Runtime::Core::Manager::GraphicDeviceManager::CreateVulkanInstance()
{
	_vkInstance = VK_NULL_HANDLE;

	vkb::InstanceBuilder instanceBuilder;	
	instanceBuilder = instanceBuilder
		.set_engine_name("AiEngine")
		.set_engine_version(VK_VERSION_1_3)
		.set_app_name("AiEngine")
		.set_app_version(VK_VERSION_1_3)
		.require_api_version(VKB_VK_API_VERSION_1_3);
#ifdef NDEBUG
	instanceBuilder = instanceBuilder
		.enable_validation_layers(false);
#else
	instanceBuilder = instanceBuilder
		.enable_validation_layers(true)
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
		.set_debug_callback(DebugCallback);
#endif

	auto vkbResult = instanceBuilder.build();
	if (!vkbResult) qFatal("Create vulkan instance failed.");

	_vkbInstance = vkbResult.value();
	_vkInstance = _vkbInstance.instance;
}

void AirEngine::Runtime::Core::Manager::GraphicDeviceManager::CreateDevice()
{
	const bool isWindow{ RenderManager::FrontEnd().IsWindow() };

	VkPhysicalDeviceShaderAtomicFloatFeaturesEXT vkPhysicalDeviceShaderAtomicFloatFeaturesEXT{};
	vkPhysicalDeviceShaderAtomicFloatFeaturesEXT.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_SHADER_ATOMIC_FLOAT_FEATURES_EXT;
	vkPhysicalDeviceShaderAtomicFloatFeaturesEXT.pNext = nullptr;
	vkPhysicalDeviceShaderAtomicFloatFeaturesEXT.shaderSharedFloat32Atomics = VK_TRUE;

	VkPhysicalDeviceSynchronization2Features vkPhysicalDeviceSynchronization2Features{};
	vkPhysicalDeviceSynchronization2Features.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_SYNCHRONIZATION_2_FEATURES;
	vkPhysicalDeviceSynchronization2Features.pNext = nullptr;
	vkPhysicalDeviceSynchronization2Features.synchronization2 = VK_TRUE;

	vkb::PhysicalDeviceSelector physicalDeviceSelector(_vkbInstance);
	physicalDeviceSelector = physicalDeviceSelector
		.add_required_extension(VK_EXT_SHADER_ATOMIC_FLOAT_EXTENSION_NAME)
		.add_required_extension_features(vkPhysicalDeviceShaderAtomicFloatFeaturesEXT)
		.add_required_extension(VK_KHR_SYNCHRONIZATION_2_EXTENSION_NAME)
		.add_required_extension_features(vkPhysicalDeviceSynchronization2Features);
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
				customQueueDescriptions.push_back(vkb::CustomQueueDescription(i, 2, { 1.0f, 1.0f }));
				graphicQueueFamily = i;
				presentQueueFamily = i;
			}
			else
			{
				customQueueDescriptions.push_back(vkb::CustomQueueDescription(i, 1, { 1.0f }));
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


	VkQueue queue = VK_NULL_HANDLE;

	vkGetDeviceQueue(_vkDevice, graphicQueueFamily, 0, &queue);
	_queueMap.insert(std::make_pair(Utility::InternedString("GraphicQueue"), new Graphic::Instance::Queue(queue, graphicQueueFamily, Utility::InternedString("GraphicQueue"))));

	vkGetDeviceQueue(_vkDevice, transferQueueFamily, 0, &queue);
	_queueMap.insert(std::make_pair(Utility::InternedString("TransferQueue"), new Graphic::Instance::Queue(queue, transferQueueFamily, Utility::InternedString("TransferQueue"))));

	if (isWindow)
	{
		vkGetDeviceQueue(_vkDevice, graphicQueueFamily, 1, &queue);
		_queueMap.insert(std::make_pair(Utility::InternedString("PresentQueue"), new Graphic::Instance::Queue(queue, graphicQueueFamily, Utility::InternedString("GraphicQueue"))));
	}
}

void AirEngine::Runtime::Core::Manager::GraphicDeviceManager::CreateMemoryAllocator()
{
	VmaVulkanFunctions vulkanFunctions = {};
	vulkanFunctions.vkGetInstanceProcAddr = &vkGetInstanceProcAddr;
	vulkanFunctions.vkGetDeviceProcAddr = &vkGetDeviceProcAddr;

	VmaAllocatorCreateInfo allocatorCreateInfo{};
	allocatorCreateInfo.vulkanApiVersion = VKB_VK_API_VERSION_1_3;
	allocatorCreateInfo.physicalDevice = _vkPhysicalDevice;
	allocatorCreateInfo.device = _vkDevice;
	allocatorCreateInfo.instance = _vkInstance;
	allocatorCreateInfo.pVulkanFunctions = &vulkanFunctions;

	vmaCreateAllocator(&allocatorCreateInfo, &_vmaAllocator);
}

void AirEngine::Runtime::Core::Manager::GraphicDeviceManager::PopulateVulkanStringToEnumMap()
{
	{
		const std::vector<uint32_t> VK_FORMAT_ENUM_FIRST_INDEX_GROUP{0, 1000156000, 1000330000, 1000340000, 1000066000, 1000054000, 1000464000};
		const std::vector<uint32_t> VK_FORMAT_ENUM_LAST_INDEX_GROUP{184, 1000156033, 1000330003, 1000340001, 1000066013, 1000054007, 1000464000};

		const auto&& GROUP_COUNT = VK_FORMAT_ENUM_FIRST_INDEX_GROUP.size();

		for (uint32_t groupIndex = 0; groupIndex < GROUP_COUNT; groupIndex++)
		{
			for (uint32_t formatIndex = VK_FORMAT_ENUM_FIRST_INDEX_GROUP[groupIndex]; formatIndex <= VK_FORMAT_ENUM_LAST_INDEX_GROUP[groupIndex]; formatIndex++)
			{
				VkFormat format = VkFormat(formatIndex);
				std::string formatString = string_VkFormat(format);
				_vkFormatStringToEnumMap[formatString] = format;
			}
		}
	}

	{
		_vkImageUsageFlagBitsStringToEnumMap["VK_IMAGE_USAGE_TRANSFER_SRC_BIT"] = VK_IMAGE_USAGE_TRANSFER_SRC_BIT;
		_vkImageUsageFlagBitsStringToEnumMap["VK_IMAGE_USAGE_TRANSFER_DST_BIT"] = VK_IMAGE_USAGE_TRANSFER_DST_BIT;
		_vkImageUsageFlagBitsStringToEnumMap["VK_IMAGE_USAGE_SAMPLED_BIT"] = VK_IMAGE_USAGE_SAMPLED_BIT;
		_vkImageUsageFlagBitsStringToEnumMap["VK_IMAGE_USAGE_STORAGE_BIT"] = VK_IMAGE_USAGE_STORAGE_BIT;
		_vkImageUsageFlagBitsStringToEnumMap["VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT"] = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;
		_vkImageUsageFlagBitsStringToEnumMap["VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT"] = VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT;
		_vkImageUsageFlagBitsStringToEnumMap["VK_IMAGE_USAGE_TRANSIENT_ATTACHMENT_BIT"] = VK_IMAGE_USAGE_TRANSIENT_ATTACHMENT_BIT;
		_vkImageUsageFlagBitsStringToEnumMap["VK_IMAGE_USAGE_INPUT_ATTACHMENT_BIT"] = VK_IMAGE_USAGE_INPUT_ATTACHMENT_BIT;
	}

	{
		_vkMemoryPropertyFlagBitsStringToEnumMap["VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT"] = VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT;
		_vkMemoryPropertyFlagBitsStringToEnumMap["VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT"] = VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT;
		_vkMemoryPropertyFlagBitsStringToEnumMap["VK_MEMORY_PROPERTY_HOST_COHERENT_BIT"] = VK_MEMORY_PROPERTY_HOST_COHERENT_BIT;
		_vkMemoryPropertyFlagBitsStringToEnumMap["VK_MEMORY_PROPERTY_HOST_CACHED_BIT"] = VK_MEMORY_PROPERTY_HOST_CACHED_BIT;
		_vkMemoryPropertyFlagBitsStringToEnumMap["VK_MEMORY_PROPERTY_LAZILY_ALLOCATED_BIT"] = VK_MEMORY_PROPERTY_LAZILY_ALLOCATED_BIT;
		_vkMemoryPropertyFlagBitsStringToEnumMap["VK_MEMORY_PROPERTY_PROTECTED_BIT"] = VK_MEMORY_PROPERTY_PROTECTED_BIT;
		_vkMemoryPropertyFlagBitsStringToEnumMap["VK_MEMORY_PROPERTY_DEVICE_COHERENT_BIT_AMD"] = VK_MEMORY_PROPERTY_DEVICE_COHERENT_BIT_AMD;
		_vkMemoryPropertyFlagBitsStringToEnumMap["VK_MEMORY_PROPERTY_DEVICE_UNCACHED_BIT_AMD"] = VK_MEMORY_PROPERTY_DEVICE_UNCACHED_BIT_AMD;
		_vkMemoryPropertyFlagBitsStringToEnumMap["VK_MEMORY_PROPERTY_RDMA_CAPABLE_BIT_NV"] = VK_MEMORY_PROPERTY_RDMA_CAPABLE_BIT_NV;
	}
}

AirEngine::Runtime::Core::Manager::GraphicDeviceManager::GraphicDeviceManager()
	: ManagerBase("GraphicDeviceManager")
{
}

AirEngine::Runtime::Core::Manager::GraphicDeviceManager::~GraphicDeviceManager()
{
}
