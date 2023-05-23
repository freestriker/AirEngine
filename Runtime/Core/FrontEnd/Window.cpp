#include "Window.hpp"
#include "../Manager/GraphicDeviceManager.hpp"


void AirEngine::Runtime::Core::FrontEnd::Window::OnPreparePresent()
{
}

void AirEngine::Runtime::Core::FrontEnd::Window::OnPresent()
{
}

void AirEngine::Runtime::Core::FrontEnd::Window::OnFinishPresent()
{
}

void AirEngine::Runtime::Core::FrontEnd::Window::OnSetVulkanHandle()
{
	setVulkanInstance(&Manager::GraphicDeviceManager::QVulkanInstance());
	_vkSurface = Manager::GraphicDeviceManager::QVulkanInstance().surfaceForWindow(this);
}

void AirEngine::Runtime::Core::FrontEnd::Window::OnCreateVulkanSwapchain()
{
	vkb::SwapchainBuilder swapchainBuilder(Manager::GraphicDeviceManager::VkbDevice());
	auto swapchainResult = swapchainBuilder.set_desired_format({ VK_FORMAT_B8G8R8A8_SRGB, VK_COLOR_SPACE_SRGB_NONLINEAR_KHR })
		.set_desired_present_mode(VK_PRESENT_MODE_FIFO_KHR)
		.set_pre_transform_flags(VK_SURFACE_TRANSFORM_IDENTITY_BIT_KHR)
		.set_composite_alpha_flags(VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR)
		.set_clipped(false)
		.set_image_array_layer_count(1)
		.set_image_usage_flags(VkImageUsageFlagBits::VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT | VkImageUsageFlagBits::VK_IMAGE_USAGE_TRANSFER_DST_BIT)
		.build();
	_vkbSwapchain = swapchainResult.value();
	_vkSwapchain = _vkbSwapchain.swapchain;
}

AirEngine::Runtime::Core::FrontEnd::Window::Window()
	: QWindow()
	, WindowFrontEndBase()
{
	setMinimumSize({ 1600, 900 });
	setMaximumSize({ 1600, 900 });
	//resize(1600, 900);
	setSurfaceType(QSurface::VulkanSurface);
	show();
}
