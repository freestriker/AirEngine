#include "RenderManager.hpp"
#include <iostream>
#include <vk_mem_alloc.h>
#include "AirEngine/Runtime/Core/FrontEnd/FrontEndBase.hpp"
#include <vulkan/vk_enum_string_helper.h>
#include "AirEngine/Runtime/Graphic/Manager/ShaderManager.hpp"
#include "AirEngine/Runtime/Graphic/Manager/DescriptorManager.hpp"
#include "AirEngine/Runtime/Core/FrontEnd/Window.hpp"
#include "AirEngine/Runtime/Core/FrontEnd/DummyWindow.hpp"

VULKAN_HPP_DEFAULT_DISPATCH_LOADER_DYNAMIC_STORAGE

vkb::Instance AirEngine::Runtime::Core::Manager::RenderManager::_vkbInstance{};
vkb::PhysicalDevice AirEngine::Runtime::Core::Manager::RenderManager::_vkbPhysicalDevice{};
vkb::Device AirEngine::Runtime::Core::Manager::RenderManager::_vkbDevice{};

vk::Instance AirEngine::Runtime::Core::Manager::RenderManager::_instance{};
vk::PhysicalDevice AirEngine::Runtime::Core::Manager::RenderManager::_physicalDevice{};
vk::Device AirEngine::Runtime::Core::Manager::RenderManager::_device{};

VmaAllocator AirEngine::Runtime::Core::Manager::RenderManager::_vmaAllocator{ VK_NULL_HANDLE };

std::unordered_map<AirEngine::Runtime::Utility::InternedString, std::unique_ptr<AirEngine::Runtime::Graphic::Instance::Queue>> AirEngine::Runtime::Core::Manager::RenderManager::_queueMap{ };

AirEngine::Runtime::Core::FrontEnd::FrontEndBase* AirEngine::Runtime::Core::Manager::RenderManager::_frontEnd{ nullptr };

#ifndef NDEBUG
static VKAPI_ATTR VkBool32 VKAPI_CALL DebugCallback(VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity, VkDebugUtilsMessageTypeFlagsEXT messageType, const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData, void* pUserData)
{
	std::string s = pCallbackData->pMessage;
	s = "Exception: " + s + "\n";
	std::cout << s;
	return VK_FALSE;
}
#endif // !NDEBUG

std::vector<AirEngine::Runtime::Utility::OperationWrapper> AirEngine::Runtime::Core::Manager::RenderManager::OnGetInitializeOperations()
{
	return {
		{ GRAPHIC_INITIALIZE_LAYER, GRAPHIC_INITIALIZE_INSTANCE_INDEX, CreateVulkanInstance },
		{ GRAPHIC_INITIALIZE_LAYER, GRAPHIC_INITIALIZE_WINDOW_SURFACE_INDEX, CreateSurface },
		{ GRAPHIC_INITIALIZE_LAYER, GRAPHIC_INITIALIZE_DEVICE_INDEX, []()->void { CreateDevice(); SetDefaultDispatcher(); PopulateQueue(); }},
		{ GRAPHIC_INITIALIZE_LAYER, GRAPHIC_INITIALIZE_SWAPCHAIN_INDEX, CreateSwapchain },
		{ GRAPHIC_INITIALIZE_LAYER, GRAPHIC_INITIALIZE_ORTHER_INDEX, []()->void { CreateMemoryAllocator(); InitializeGraphicManagers(); }},
	};
}

void AirEngine::Runtime::Core::Manager::RenderManager::CreateVulkanInstance()
{
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
	_instance = vk::Instance(_vkbInstance.instance);
}

void AirEngine::Runtime::Core::Manager::RenderManager::CreateSurface()
{
	//_frontEnd = new FrontEnd::DummyWindow();
	_frontEnd = new FrontEnd::Window();
	_frontEnd->OnCreateSurface();
}

void AirEngine::Runtime::Core::Manager::RenderManager::CreateDevice()
{
	const bool isWindow{ FrontEnd().IsWindow() };

	VkPhysicalDeviceShaderAtomicFloatFeaturesEXT vkPhysicalDeviceShaderAtomicFloatFeaturesEXT{};
	vkPhysicalDeviceShaderAtomicFloatFeaturesEXT.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_SHADER_ATOMIC_FLOAT_FEATURES_EXT;
	vkPhysicalDeviceShaderAtomicFloatFeaturesEXT.pNext = nullptr;
	vkPhysicalDeviceShaderAtomicFloatFeaturesEXT.shaderSharedFloat32Atomics = VK_TRUE;

	VkPhysicalDeviceSynchronization2Features vkPhysicalDeviceSynchronization2Features{};
	vkPhysicalDeviceSynchronization2Features.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_SYNCHRONIZATION_2_FEATURES;
	vkPhysicalDeviceSynchronization2Features.pNext = nullptr;
	vkPhysicalDeviceSynchronization2Features.synchronization2 = VK_TRUE;

	VkPhysicalDeviceIndexTypeUint8FeaturesEXT vkPhysicalDeviceIndexTypeUint8FeaturesEXT{};
	vkPhysicalDeviceIndexTypeUint8FeaturesEXT.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_INDEX_TYPE_UINT8_FEATURES_EXT;
	vkPhysicalDeviceIndexTypeUint8FeaturesEXT.pNext = nullptr;
	vkPhysicalDeviceIndexTypeUint8FeaturesEXT.indexTypeUint8 = VK_TRUE;

	VkPhysicalDeviceVertexInputDynamicStateFeaturesEXT vkPhysicalDeviceVertexInputDynamicStateFeaturesEXT{};
	vkPhysicalDeviceVertexInputDynamicStateFeaturesEXT.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_VERTEX_INPUT_DYNAMIC_STATE_FEATURES_EXT;
	vkPhysicalDeviceVertexInputDynamicStateFeaturesEXT.pNext = nullptr;
	vkPhysicalDeviceVertexInputDynamicStateFeaturesEXT.vertexInputDynamicState = true;

	VkPhysicalDeviceBufferDeviceAddressFeatures vkPhysicalDeviceBufferDeviceAddressFeatures{};
	vkPhysicalDeviceBufferDeviceAddressFeatures.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_BUFFER_DEVICE_ADDRESS_FEATURES;
	vkPhysicalDeviceBufferDeviceAddressFeatures.pNext = nullptr;
	vkPhysicalDeviceBufferDeviceAddressFeatures.bufferDeviceAddress = VK_TRUE;

	VkPhysicalDeviceDescriptorBufferFeaturesEXT vkPhysicalDeviceDescriptorBufferFeaturesEXT{};
	vkPhysicalDeviceDescriptorBufferFeaturesEXT.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_DESCRIPTOR_BUFFER_FEATURES_EXT;
	vkPhysicalDeviceDescriptorBufferFeaturesEXT.pNext = nullptr;
	vkPhysicalDeviceDescriptorBufferFeaturesEXT.descriptorBuffer = VK_TRUE;

	VkPhysicalDeviceDescriptorIndexingFeaturesEXT vkPhysicalDeviceDescriptorIndexingFeaturesEXT{};
	vkPhysicalDeviceDescriptorIndexingFeaturesEXT.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_DESCRIPTOR_INDEXING_FEATURES_EXT;
	vkPhysicalDeviceDescriptorIndexingFeaturesEXT.shaderUniformBufferArrayNonUniformIndexing = VK_TRUE;
	vkPhysicalDeviceDescriptorIndexingFeaturesEXT.shaderStorageBufferArrayNonUniformIndexing = VK_TRUE;
	vkPhysicalDeviceDescriptorIndexingFeaturesEXT.shaderSampledImageArrayNonUniformIndexing = VK_TRUE;
	vkPhysicalDeviceDescriptorIndexingFeaturesEXT.shaderStorageImageArrayNonUniformIndexing = VK_TRUE;
	vkPhysicalDeviceDescriptorIndexingFeaturesEXT.shaderInputAttachmentArrayNonUniformIndexing = VK_TRUE;
	vkPhysicalDeviceDescriptorIndexingFeaturesEXT.runtimeDescriptorArray = VK_TRUE;
	vkPhysicalDeviceDescriptorIndexingFeaturesEXT.descriptorBindingVariableDescriptorCount = VK_TRUE;

	vkb::PhysicalDeviceSelector physicalDeviceSelector(_vkbInstance);
	physicalDeviceSelector = physicalDeviceSelector
		.add_required_extension(VK_EXT_SHADER_ATOMIC_FLOAT_EXTENSION_NAME)
		.add_required_extension_features(vkPhysicalDeviceShaderAtomicFloatFeaturesEXT)
		.add_required_extension(VK_KHR_SYNCHRONIZATION_2_EXTENSION_NAME)
		.add_required_extension_features(vkPhysicalDeviceSynchronization2Features)
		.add_required_extension(VK_EXT_INDEX_TYPE_UINT8_EXTENSION_NAME)
		.add_required_extension_features(vkPhysicalDeviceIndexTypeUint8FeaturesEXT)
		.add_required_extension(VK_EXT_VERTEX_INPUT_DYNAMIC_STATE_EXTENSION_NAME)
		.add_required_extension_features(vkPhysicalDeviceVertexInputDynamicStateFeaturesEXT)
		.add_required_extension(VK_KHR_BUFFER_DEVICE_ADDRESS_EXTENSION_NAME)
		.add_required_extension_features(vkPhysicalDeviceBufferDeviceAddressFeatures)
		.add_required_extension(VK_EXT_DESCRIPTOR_BUFFER_EXTENSION_NAME)
		.add_required_extension_features(vkPhysicalDeviceDescriptorBufferFeaturesEXT)
		.add_required_extension(VK_EXT_DESCRIPTOR_INDEXING_EXTENSION_NAME)
		.add_required_extension_features(vkPhysicalDeviceDescriptorIndexingFeaturesEXT);
	if (isWindow)
	{
		physicalDeviceSelector.set_surface(dynamic_cast<FrontEnd::WindowFrontEndBase&>(FrontEnd()).VkSurface());
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
	_physicalDevice = vk::PhysicalDevice(_vkbPhysicalDevice.physical_device);

	std::vector<vkb::CustomQueueDescription> customQueueDescriptions;
	uint32_t graphicQueueFamily = 0;
	//uint32_t transferQueueFamily = 0;
	//uint32_t presentQueueFamily = 0;
	auto queue_families = _vkbPhysicalDevice.get_queue_families();
	for (uint32_t i = 0; i < (uint32_t)queue_families.size(); i++) 
	{
		if (queue_families[i].queueFlags & VK_QUEUE_GRAPHICS_BIT) 
		{
			if (isWindow)
			{
				customQueueDescriptions.push_back(vkb::CustomQueueDescription(i, 3, { 1.0f, 1.0f, 1.0f }));
				graphicQueueFamily = i;
				//presentQueueFamily = i;
			}
			else
			{
				customQueueDescriptions.push_back(vkb::CustomQueueDescription(i, 2, { 1.0f, 1.0f }));
				//presentQueueFamily = i;
			}
			break;
		}
	}
	//if (_vkbPhysicalDevice.has_dedicated_transfer_queue()) 
	//{
	//	for (uint32_t i = 0; i < (uint32_t)queue_families.size(); i++) 
	//	{
	//		if ((queue_families[i].queueFlags & VK_QUEUE_TRANSFER_BIT) &&
	//			(queue_families[i].queueFlags & VK_QUEUE_GRAPHICS_BIT) == 0 &&
	//			(queue_families[i].queueFlags & VK_QUEUE_COMPUTE_BIT) == 0)
	//		{
	//			customQueueDescriptions.push_back(vkb::CustomQueueDescription(i, 1, { 1.0f }));
	//			transferQueueFamily = i;
	//			break;
	//		}
	//	}
	//}
	//else if (_vkbPhysicalDevice.has_separate_transfer_queue()) 
	//{
	//	for (uint32_t i = 0; i < (uint32_t)queue_families.size(); i++) 
	//	{
	//		if ((queue_families[i].queueFlags & VK_QUEUE_TRANSFER_BIT) &&
	//			((queue_families[i].queueFlags & VK_QUEUE_GRAPHICS_BIT) == 0))
	//		{
	//			customQueueDescriptions.push_back(vkb::CustomQueueDescription(i, 1, { 1.0f }));
	//			transferQueueFamily = i;
	//			break;
	//		}
	//	}
	//}

	vkb::DeviceBuilder deviceBuilder(_vkbPhysicalDevice);
	auto deviceResult = deviceBuilder
		.custom_queue_setup(customQueueDescriptions)
		.build();
	if (!deviceResult) 
	{
		qFatal((std::string("Failed to create Vulkan device. Error: ") + std::string(deviceResult.error().message())).c_str());
	}
	_vkbDevice = deviceResult.value();
	_device = vk::Device(_vkbDevice.device);
}

void AirEngine::Runtime::Core::Manager::RenderManager::SetDefaultDispatcher()
{
	vk::DynamicLoader dynamicLoader{};
	PFN_vkGetInstanceProcAddr vkGetInstanceProcAddr = dynamicLoader.getProcAddress<PFN_vkGetInstanceProcAddr>("vkGetInstanceProcAddr");
	VULKAN_HPP_DEFAULT_DISPATCHER.init(vkGetInstanceProcAddr);
	VULKAN_HPP_DEFAULT_DISPATCHER.init(Instance());
	VULKAN_HPP_DEFAULT_DISPATCHER.init(Device());
}

void AirEngine::Runtime::Core::Manager::RenderManager::PopulateQueue()
{
	const bool isWindow{ FrontEnd().IsWindow() };

	vk::Queue queue{};

	queue = _device.getQueue(0, 0);
	_queueMap.insert(std::make_pair(Utility::InternedString("GraphicQueue"), new Graphic::Instance::Queue(queue, 0, Utility::InternedString("GraphicQueue"))));
	queue = _device.getQueue(0, 1);
	_queueMap.insert(std::make_pair(Utility::InternedString("TransferQueue"), new Graphic::Instance::Queue(queue, 0, Utility::InternedString("TransferQueue"))));

	if (isWindow)
	{
		queue = _device.getQueue(0, 2);
		_queueMap.insert(std::make_pair(Utility::InternedString("PresentQueue"), new Graphic::Instance::Queue(queue, 0, Utility::InternedString("PresentQueue"))));
	}

}

void AirEngine::Runtime::Core::Manager::RenderManager::CreateSwapchain()
{
	_frontEnd->OnCreateSwapchain();
}

void AirEngine::Runtime::Core::Manager::RenderManager::CreateMemoryAllocator()
{
	VmaVulkanFunctions vulkanFunctions = {};
	vulkanFunctions.vkGetInstanceProcAddr = &vkGetInstanceProcAddr;
	vulkanFunctions.vkGetDeviceProcAddr = &vkGetDeviceProcAddr;

	VmaAllocatorCreateInfo allocatorCreateInfo{};
	allocatorCreateInfo.vulkanApiVersion = VKB_VK_API_VERSION_1_3;
	allocatorCreateInfo.physicalDevice = _vkbPhysicalDevice.physical_device;
	allocatorCreateInfo.device = _vkbDevice.device;
	allocatorCreateInfo.instance = _vkbInstance.instance;
	allocatorCreateInfo.pVulkanFunctions = &vulkanFunctions;
	allocatorCreateInfo.flags = VMA_ALLOCATOR_CREATE_BUFFER_DEVICE_ADDRESS_BIT;

	vmaCreateAllocator(&allocatorCreateInfo, &_vmaAllocator);
}

void AirEngine::Runtime::Core::Manager::RenderManager::InitializeGraphicManagers()
{
	Graphic::Manager::ShaderManager::Initialize();
	Graphic::Manager::DescriptorManager::Initialize();
}

std::vector<AirEngine::Runtime::Utility::OperationWrapper> AirEngine::Runtime::Core::Manager::RenderManager::OnGetUpdateOperations()
{
	return
	{
		{ RENDER_UPDATE_LAYER, 0, RenderUpdate }
	};
}

void AirEngine::Runtime::Core::Manager::RenderManager::RenderUpdate()
{
	if (!_frontEnd->IsReadyToRender())
	{
		return;
	}

	_frontEnd->StartRender();

	//render
	std::cout << "Render loop.\n";
	std::this_thread::sleep_for(std::chrono::milliseconds(5));

	_frontEnd->FinishRender();
}

AirEngine::Runtime::Core::Manager::RenderManager::RenderManager()
	: ManagerBase("RenderManager")
{
}

AirEngine::Runtime::Core::Manager::RenderManager::~RenderManager()
{
}
