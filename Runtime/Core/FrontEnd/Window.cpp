#include "Window.hpp"
#include "../Manager/GraphicDeviceManager.hpp"
#include <vulkan/vulkan_core.h>
#include "../../Graphic/Instance/Image.hpp"
#include "../../Graphic/Command/Semaphore.hpp"
#include "../../Graphic/Command/Fence.hpp"
#include "../../Graphic/Command/CommandPool.hpp"

void AirEngine::Runtime::Core::FrontEnd::Window::OnCreate()
{
	setMinimumSize({ 1600, 900 });
	setMaximumSize({ 1600, 900 });
	//resize(1600, 900);
	setSurfaceType(QSurface::VulkanSurface);
	show();
}

void AirEngine::Runtime::Core::FrontEnd::Window::OnPreparePresent()
{
}

void AirEngine::Runtime::Core::FrontEnd::Window::OnPresent()
{
	auto&& currentFrame = _frames[_swapchainCurrentFrameIndex];
	uint32_t nextImageIndex = -1;
	vkAcquireNextImageKHR(
		Manager::GraphicDeviceManager::VkDevice(), 
		_vkSwapchain, 
		std::numeric_limits<uint64_t>::max(), 
		currentFrame.imageSemaphore->VkHandle(),
		currentFrame.fence->VkHandle(),
		&nextImageIndex
	);
	currentFrame.imageSemWaitable = true;
	currentFrame.imageAcquired = true;
	currentFrame.fenceWaitable = true;


}

void AirEngine::Runtime::Core::FrontEnd::Window::OnFinishPresent()
{
}

void AirEngine::Runtime::Core::FrontEnd::Window::OnSetVulkanHandle()
{
	_qVulkanInstance.setVkInstance(Manager::GraphicDeviceManager::VkInstance());
	bool qResult = _qVulkanInstance.create();
	if (!qResult) qFatal("Create vulkan instance failed.");

	setVulkanInstance(&_qVulkanInstance);
	_vkSurface = _qVulkanInstance.surfaceForWindow(this);
}

void AirEngine::Runtime::Core::FrontEnd::Window::OnCreateVulkanSwapchain()
{
	vkb::SwapchainBuilder swapchainBuilder(Manager::GraphicDeviceManager::VkbDevice());
	swapchainBuilder = swapchainBuilder.set_desired_format({ VK_FORMAT_B8G8R8A8_SRGB, VK_COLOR_SPACE_SRGB_NONLINEAR_KHR })
		.set_desired_present_mode(VK_PRESENT_MODE_FIFO_KHR)
		.set_pre_transform_flags(VK_SURFACE_TRANSFORM_IDENTITY_BIT_KHR)
		.set_composite_alpha_flags(VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR)
		.set_clipped(false)
		.set_image_array_layer_count(1)
		.set_required_min_image_count(3)
		.set_image_usage_flags(VkImageUsageFlagBits::VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT | VkImageUsageFlagBits::VK_IMAGE_USAGE_TRANSFER_DST_BIT);
	auto swapchainResult = swapchainBuilder.build();
	
	_vkbSwapchain = swapchainResult.value();
	_vkSwapchain = _vkbSwapchain.swapchain;
	
	auto vkImages = _vkbSwapchain.get_images().value();
	_swapchainImages.reserve(vkImages.size());
	_frames.reserve(vkImages.size());
	for (auto& vkImage : vkImages)
	{
		_swapchainImages.emplace_back(
			Graphic::Instance::Image::CreateSwapchainImage(
				vkImage,
				_vkbSwapchain.image_format,
				_vkbSwapchain.extent,
				_vkbSwapchain.image_usage_flags
			)
		);
		_frames.emplace_back(
			new Graphic::Command::Fence(false),
			false,
			new Graphic::Command::Semaphore(),
			new Graphic::Command::Semaphore(),
			new Graphic::Command::Semaphore(),
			false,
			false
		);
	}

	_commandPool = new Graphic::Command::CommandPool(Utility::InternedString("PresentQueue"));
}

AirEngine::Runtime::Core::FrontEnd::Window::Window()
	: QWindow()
	, WindowFrontEndBase()
	, _qVulkanInstance()
	, _swapchainImages()
	, _swapchainCurrentFrameIndex(0)
{
}
