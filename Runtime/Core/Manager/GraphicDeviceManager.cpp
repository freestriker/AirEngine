#include "GraphicDeviceManager.hpp"
#include <iostream>
#include <mutex>
#include <QWindow>

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
		{ 0, 3, CreateSwapchain },
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
		.require_dedicated_transfer_queue()
		.require_separate_compute_queue()
		.require_present(true)
		.select();
	if (!physicalDeviceResult) {
		qFatal((std::string("Failed to select Vulkan Physical Device. Error: ") + std::string(physicalDeviceResult.error().message())).c_str());
	}
	_vkbPhysicalDevice = physicalDeviceResult.value();
	_vkPhysicalDevice = _vkbPhysicalDevice.physical_device;

	vkb::DeviceBuilder deviceBuilder(_vkbPhysicalDevice);
	auto deviceResult = deviceBuilder
		.build();
	if (!deviceResult) {
		qFatal((std::string("Failed to create Vulkan device. Error: ") + std::string(deviceResult.error().message())).c_str());
	}
	_vkbDevice = deviceResult.value();
	_vkDevice = _vkbDevice.device;
}

void AirEngine::Runtime::Core::Manager::GraphicDeviceManager::CreateSwapchain()
{
	vkb::SwapchainBuilder swapchainBuilder(_vkbDevice);
	auto swapchainResult = swapchainBuilder.build();
	_vkbSwapchain = swapchainResult.value();
	_vkSwapchain = _vkbSwapchain.swapchain;
}

AirEngine::Runtime::Core::Manager::GraphicDeviceManager::GraphicDeviceManager()
	: ManagerBase("GraphicDeviceManager")
{
}

AirEngine::Runtime::Core::Manager::GraphicDeviceManager::~GraphicDeviceManager()
{
}
