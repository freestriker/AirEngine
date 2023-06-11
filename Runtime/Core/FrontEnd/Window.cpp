#include "Window.hpp"
#include "../Manager/GraphicDeviceManager.hpp"
#include <vulkan/vulkan_core.h>
#include "../../Graphic/Instance/Image.hpp"
#include "../../Graphic/Command/Semaphore.hpp"
#include "../../Graphic/Command/Fence.hpp"
#include "../../Graphic/Command/Barrier.hpp"
#include "../../Graphic/Command/CommandPool.hpp"
#include "../../Graphic/Command/CommandBuffer.hpp"
#include "../../Graphic/Instance/Queue.hpp"
#include "../../AssetLoader/Texture2DLoader.hpp"
#include "../../Asset/Texture2D.hpp"

void AirEngine::Runtime::Core::FrontEnd::Window::OnCreate()
{
	setMinimumSize({ 1600, 900 });
	setMaximumSize({ 1600, 900 });
	//resize(1600, 900);
	setSurfaceType(QSurface::VulkanSurface);
	show();
}

void AirEngine::Runtime::Core::FrontEnd::Window::OnAcquireImage()
{    
	auto&& currentFrame = _frameResources[_curFrameIndex];

	while (currentFrame.acquireFence->Status() == VK_NOT_READY) Utility::ThisFiber::yield();
	currentFrame.acquireFence->Reset();

	vkAcquireNextImageKHR(
		Manager::GraphicDeviceManager::VkDevice(),
		_vkSwapchain,
		std::numeric_limits<uint64_t>::max(),
		currentFrame.acquireSemaphore->VkHandle(),
		currentFrame.acquireFence->VkHandle(),
		&_curImageIndex
	);
}
static AirEngine::Runtime::AssetLoader::Texture2DLoader* texture2DLoader = nullptr;
static AirEngine::Runtime::AssetLoader::AssetLoadHandle assetLoadHandle{};

void AirEngine::Runtime::Core::FrontEnd::Window::OnPresent()
{
	if (texture2DLoader == nullptr)
	{
		texture2DLoader = new AssetLoader::Texture2DLoader();
		assetLoadHandle = texture2DLoader->LoadAsset("..\\../Resources\\Texture/WorkShop_Equirectangular.texture2d");
	}

	auto&& currentFrame = _frameResources[_curFrameIndex];
	auto&& currentImage = _imageResources[_curImageIndex];

	_commandPool->Reset();
	_commandBuffer->BeginRecord(VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT);
	Graphic::Command::Barrier barrier{};
	if (assetLoadHandle.IsCompleted())
	{
		barrier.AddImageMemoryBarrier(
			*currentImage.image,
			VK_PIPELINE_STAGE_2_NONE,
			VK_ACCESS_2_NONE,
			VK_PIPELINE_STAGE_2_BLIT_BIT,
			VK_ACCESS_2_TRANSFER_WRITE_BIT,
			VkImageLayout::VK_IMAGE_LAYOUT_UNDEFINED,
			VkImageLayout::VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
			VkImageAspectFlagBits::VK_IMAGE_ASPECT_COLOR_BIT
		);
		_commandBuffer->AddPipelineBarrier(barrier);
		_commandBuffer->Blit(
			assetLoadHandle.Asset<Asset::Texture2D>().Image(), VkImageLayout::VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL,
			*currentImage.image, VkImageLayout::VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
			VkImageAspectFlagBits::VK_IMAGE_ASPECT_COLOR_BIT,
			VkFilter::VK_FILTER_LINEAR
		);
		barrier.ClearImageMemoryBarriers();
		barrier.AddImageMemoryBarrier(
			*currentImage.image,
			VK_PIPELINE_STAGE_2_BLIT_BIT,
			VK_ACCESS_2_TRANSFER_WRITE_BIT,
			VK_PIPELINE_STAGE_2_NONE,
			VK_ACCESS_2_NONE,
			VkImageLayout::VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
			VkImageLayout::VK_IMAGE_LAYOUT_PRESENT_SRC_KHR,
			VkImageAspectFlagBits::VK_IMAGE_ASPECT_COLOR_BIT
		);
		_commandBuffer->AddPipelineBarrier(barrier);
	}
	else
	{
		barrier.AddImageMemoryBarrier(
			*currentImage.image,
			VK_PIPELINE_STAGE_2_NONE,
			VK_ACCESS_2_NONE,
			VK_PIPELINE_STAGE_2_CLEAR_BIT,
			VK_ACCESS_2_TRANSFER_WRITE_BIT,
			VkImageLayout::VK_IMAGE_LAYOUT_UNDEFINED,
			VkImageLayout::VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
			VkImageAspectFlagBits::VK_IMAGE_ASPECT_COLOR_BIT
		);
		_commandBuffer->AddPipelineBarrier(barrier);
		_commandBuffer->ClearColorImage<float>(*currentImage.image, VkImageLayout::VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, { 1.0f, 1.0f, 1.0f, 1.0f });
		barrier.ClearImageMemoryBarriers();
		barrier.AddImageMemoryBarrier(
			*currentImage.image,
			VK_PIPELINE_STAGE_2_CLEAR_BIT,
			VK_ACCESS_2_TRANSFER_WRITE_BIT,
			VK_PIPELINE_STAGE_2_NONE,
			VK_ACCESS_2_NONE,
			VkImageLayout::VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
			VkImageLayout::VK_IMAGE_LAYOUT_PRESENT_SRC_KHR,
			VkImageAspectFlagBits::VK_IMAGE_ASPECT_COLOR_BIT
		);
		_commandBuffer->AddPipelineBarrier(barrier);
	}
	_commandBuffer->EndRecord();

	_commandPool->Queue().ImmediateIndividualSubmit(
		{
			{{currentFrame.acquireSemaphore, VK_PIPELINE_STAGE_2_CLEAR_BIT}},
			{_commandBuffer},
			{{currentImage.transferSemaphore, VK_PIPELINE_STAGE_2_NONE}}
		},
		*_transferFence
	);

	VkPresentInfoKHR vkPresentInfo{};
	vkPresentInfo.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
	vkPresentInfo.swapchainCount = 1;
	vkPresentInfo.pSwapchains = &_vkSwapchain;
	vkPresentInfo.pImageIndices = &_curImageIndex;
	vkPresentInfo.waitSemaphoreCount = 1;
	vkPresentInfo.pWaitSemaphores = &currentImage.transferSemaphore->VkHandle();
	vkQueuePresentKHR(_commandPool->Queue().VkHandle(), &vkPresentInfo);

	while (_transferFence->Status() == VK_NOT_READY) Utility::ThisFiber::yield();
	_transferFence->Reset();

	_curFrameIndex = (_curFrameIndex + 1) % 3;
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
	_frameResources.reserve(vkImages.size());
	_imageResources.reserve(vkImages.size());
	for (auto& vkImage : vkImages)
	{
		_frameResources.emplace_back(
			new Graphic::Command::Fence(true),
			new Graphic::Command::Semaphore()
		);
		_imageResources.emplace_back(
			Graphic::Instance::Image::CreateSwapchainImage(
				vkImage,
				_vkbSwapchain.image_format,
				_vkbSwapchain.extent,
				_vkbSwapchain.image_usage_flags
			),
			new Graphic::Command::Semaphore()
		);
	}

	_commandPool = new Graphic::Command::CommandPool(Utility::InternedString("PresentQueue"), VK_COMMAND_POOL_CREATE_TRANSIENT_BIT);
	_commandBuffer = &_commandPool->CreateCommandBuffer(Utility::InternedString("PresentCommandBuffer"));

	_transferFence = new Graphic::Command::Fence(false);
}

AirEngine::Runtime::Core::FrontEnd::Window::Window()
	: QWindow()
	, WindowFrontEndBase()
	, _qVulkanInstance()
	, _frameResources()
	, _imageResources()
	, _curFrameIndex(0)
	, _curImageIndex(0)
	, _commandPool(nullptr)
	, _commandBuffer(nullptr)
	, _transferFence(nullptr)
{
}
