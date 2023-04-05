#include "GraphicDeviceManager.hpp"
#include <iostream>
#include <mutex>
#include <QWindow>
#include <vk_mem_alloc.h>

VkInstance AirEngine::Runtime::Core::Manager::GraphicDeviceManager::_vkInstance = VK_NULL_HANDLE;
VkSurfaceKHR AirEngine::Runtime::Core::Manager::GraphicDeviceManager::_vkSurface{ VK_NULL_HANDLE };
VkPhysicalDevice AirEngine::Runtime::Core::Manager::GraphicDeviceManager::_vkPhysicalDevice{ VK_NULL_HANDLE };
VkDevice AirEngine::Runtime::Core::Manager::GraphicDeviceManager::_vkDevice{ VK_NULL_HANDLE };
VkSwapchainKHR AirEngine::Runtime::Core::Manager::GraphicDeviceManager::_vkSwapchain{ VK_NULL_HANDLE };

QVulkanInstance AirEngine::Runtime::Core::Manager::GraphicDeviceManager::_qVulkanInstance{};
QWindow* AirEngine::Runtime::Core::Manager::GraphicDeviceManager::_window{ nullptr };

vkb::Instance AirEngine::Runtime::Core::Manager::GraphicDeviceManager::_vkbInstance{};
vkb::PhysicalDevice AirEngine::Runtime::Core::Manager::GraphicDeviceManager::_vkbPhysicalDevice{};
vkb::Device AirEngine::Runtime::Core::Manager::GraphicDeviceManager::_vkbDevice{};
vkb::Swapchain AirEngine::Runtime::Core::Manager::GraphicDeviceManager::_vkbSwapchain{};

VmaAllocator AirEngine::Runtime::Core::Manager::GraphicDeviceManager::_vmaAllocator{ VK_NULL_HANDLE };

std::unordered_map<AirEngine::Runtime::Utility::InternedString, std::unique_ptr<AirEngine::Runtime::Graphic::Instance::Queue>> AirEngine::Runtime::Core::Manager::GraphicDeviceManager::_queueMap{ };

static VKAPI_ATTR VkBool32 VKAPI_CALL DebugCallback(VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity, VkDebugUtilsMessageTypeFlagsEXT messageType, const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData, void* pUserData)
{
	std::string s = pCallbackData->pMessage;
	s = "Exception: " + s + "\n";
	std::cout << s;
	return VK_FALSE;
}

std::vector<AirEngine::Runtime::Core::Boot::ManagerInitializerWrapper> AirEngine::Runtime::Core::Manager::GraphicDeviceManager::OnGetManagerInitializers()
{
	return {
		{ 0, 0, CreateVulkanInstance },
		{ 0, 1, []()->void { CreateSurfaceWindow(*new QWindow()); } },
		{ 0, 2, CreateDevice },
		{ 0, 4, CreateSwapchain },
		{ 0, 5, CreateMemoryAllocator },
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

void AirEngine::Runtime::Core::Manager::GraphicDeviceManager::CreateSurfaceWindow(QWindow& window)
{
	_window = &window;
	window.setSurfaceType(QSurface::VulkanSurface);
	window.show();
	window.setVulkanInstance(&_qVulkanInstance);
	auto same = _vkInstance == _qVulkanInstance.vkInstance();
	if (!same) qFatal("Create vulkan instance failed.");
	_vkSurface = _qVulkanInstance.surfaceForWindow(&window);
}

void AirEngine::Runtime::Core::Manager::GraphicDeviceManager::CreateDevice()
{
	vkb::PhysicalDeviceSelector physicalDeviceSelector(_vkbInstance);
	auto physicalDeviceResult = physicalDeviceSelector
		.set_surface(_vkSurface)
		.require_present(true)
		.select();
	if (!physicalDeviceResult) {
		qFatal((std::string("Failed to select Vulkan Physical Device. Error: ") + std::string(physicalDeviceResult.error().message())).c_str());
	}
	_vkbPhysicalDevice = physicalDeviceResult.value();
	_vkPhysicalDevice = _vkbPhysicalDevice.physical_device;

	std::vector<vkb::CustomQueueDescription> customQueueDescriptions;
	uint32_t graphicQueueFamily = 0;
	uint32_t presentQueueFamily = 0;
	uint32_t transferQueueFamily = 0;
	auto queue_families = _vkbPhysicalDevice.get_queue_families();
	for (uint32_t i = 0; i < (uint32_t)queue_families.size(); i++) 
	{
		if (queue_families[i].queueFlags & VK_QUEUE_GRAPHICS_BIT) 
		{
			customQueueDescriptions.push_back(vkb::CustomQueueDescription(i, 2, {1.0f}));
			graphicQueueFamily = i;
			presentQueueFamily = i;
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

	_queueMap.insert(std::make_pair(Utility::InternedString("GraphicQueue"), new Graphic::Instance::Queue(_vkbDevice.get_queue(vkb::QueueType::graphics).value(), Utility::InternedString("GraphicQueue"))));
	_queueMap.insert(std::make_pair(Utility::InternedString("TransferQueue"), new Graphic::Instance::Queue(_vkbDevice.get_queue(vkb::QueueType::transfer).value(), Utility::InternedString("TransferQueue"))));
	_queueMap.insert(std::make_pair(Utility::InternedString("PresentQueue"), new Graphic::Instance::Queue(_vkbDevice.get_queue(vkb::QueueType::present).value(), Utility::InternedString("PresentQueue"))));
}

void AirEngine::Runtime::Core::Manager::GraphicDeviceManager::CreateSwapchain()
{
	vkb::SwapchainBuilder swapchainBuilder(_vkbDevice);
	auto swapchainResult = swapchainBuilder.set_desired_format({ VK_FORMAT_R8G8B8A8_UNORM, VK_COLOR_SPACE_SRGB_NONLINEAR_KHR })
		.set_desired_present_mode(VK_PRESENT_MODE_MAILBOX_KHR)
		.set_pre_transform_flags(VK_SURFACE_TRANSFORM_IDENTITY_BIT_KHR)
		.set_composite_alpha_flags(VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR)
		.set_clipped(false)
		.set_image_array_layer_count(1)
		.set_image_usage_flags(VkImageUsageFlagBits::VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT | VkImageUsageFlagBits::VK_IMAGE_USAGE_TRANSFER_DST_BIT)
		.build();
	_vkbSwapchain = swapchainResult.value();
	_vkSwapchain = _vkbSwapchain.swapchain;
}

void AirEngine::Runtime::Core::Manager::GraphicDeviceManager::CreateMemoryAllocator()
{
	VmaAllocatorCreateInfo allocatorCreateInfo{};
	//allocatorCreateInfo.vulkanApiVersion = VK_VERSION_1_3;
	allocatorCreateInfo.physicalDevice = _vkPhysicalDevice;
	allocatorCreateInfo.device = _vkDevice;
	allocatorCreateInfo.instance = _vkInstance;

	vmaCreateAllocator(&allocatorCreateInfo, &_vmaAllocator);
}

AirEngine::Runtime::Core::Manager::GraphicDeviceManager::GraphicDeviceManager()
	: ManagerBase("GraphicDeviceManager")
{
}

AirEngine::Runtime::Core::Manager::GraphicDeviceManager::~GraphicDeviceManager()
{
}
