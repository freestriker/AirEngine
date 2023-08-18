#include "Window.hpp"
#include "../Manager/GraphicDeviceManager.hpp"
#include <vulkan/vulkan.hpp>
#include "../../Graphic/Instance/Image.hpp"
#include "../../Graphic/Command/Semaphore.hpp"
#include "../../Graphic/Command/Fence.hpp"
#include "../../Graphic/Command/Barrier.hpp"
#include "../../Graphic/Command/CommandPool.hpp"
#include "../../Graphic/Command/CommandBuffer.hpp"
#include "../../Graphic/Instance/Queue.hpp"
#include "../../AssetLoader/Texture2DLoader.hpp"
#include "../../Asset/Texture2D.hpp"
#include "../../Asset/Mesh.hpp"
#include "../../Core/Manager/AssetManager.hpp"
#include "../../Graphic/Manager/RenderPassManager.hpp"
#include "../../Graphic/Manager/DescriptorManager.hpp"
#include "../../Graphic/Instance/RenderPassBase.hpp"
#include "../../Graphic/Instance/Buffer.hpp"
#include "../../Graphic/Rendering/Shader.hpp"
#include "../../Graphic/Rendering/Material.hpp"
#include "../../Graphic/Instance/UniformBuffer.hpp"
#include "../../Graphic/Manager/DescriptorManager.hpp"

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

	while (currentFrame.acquireFence->Status() == vk::Result::eNotReady) Utility::ThisFiber::yield();
	currentFrame.acquireFence->Reset();

	auto&& result = Manager::GraphicDeviceManager::Device().acquireNextImageKHR(
		_vkSwapchain,
		std::numeric_limits<uint64_t>::max(),
		currentFrame.acquireSemaphore->VkHandle(),
		currentFrame.acquireFence->VkHandle()
	);

	_curImageIndex = result.value;

	static bool isLoaded = false;
}
static bool isLoaded = false;
static AirEngine::Runtime::AssetLoader::AssetLoadHandle assetLoadHandle{};
static AirEngine::Runtime::AssetLoader::AssetLoadHandle meshLoadHandle{};
static AirEngine::Runtime::AssetLoader::AssetLoadHandle shaderLoadHandle{};

void AirEngine::Runtime::Core::FrontEnd::Window::OnPresent()
{
	if (!isLoaded)
	{
		isLoaded = true;
		assetLoadHandle = Core::Manager::AssetManager::LoadAsset("..\\../Resources\\Texture/WorkShop_Equirectangular.texture2d");
		meshLoadHandle = Core::Manager::AssetManager::LoadAsset("..\\../Resources\\Mesh/NineSphere.mesh");
		shaderLoadHandle = Core::Manager::AssetManager::LoadAsset("..\\../Resources\\Shader/Present.shader");

		{
			auto&& renderPass0 = Graphic::Manager::RenderPassManager::LoadRenderPass<Graphic::Instance::DummyRenderPass>();
			auto&& renderPass1 = Graphic::Manager::RenderPassManager::LoadRenderPass("AirEngine::Runtime::Graphic::Instance::DummyRenderPass");
			Graphic::Manager::RenderPassManager::Collect();
			Graphic::Manager::RenderPassManager::UnloadRenderPass<Graphic::Instance::DummyRenderPass>();
			Graphic::Manager::RenderPassManager::UnloadRenderPass("AirEngine::Runtime::Graphic::Instance::DummyRenderPass");
			Graphic::Manager::RenderPassManager::Collect();
		}

		auto&& stagingBuffer = Graphic::Instance::Buffer(
			32 * 1024 * 1024,
			vk::BufferUsageFlagBits::eResourceDescriptorBufferEXT | vk::BufferUsageFlagBits::eSamplerDescriptorBufferEXT | vk::BufferUsageFlagBits::eTransferDst,
			vk::MemoryPropertyFlagBits::eDeviceLocal
		);

		auto&& uniformBuffer = Graphic::Instance::UniformBuffer(
			4 * 1024 * 1024,
			vk::BufferUsageFlagBits::eUniformBuffer,
			vk::MemoryPropertyFlagBits::eDeviceLocal
		);

		//auto&& handle0 = Graphic::Manager::DescriptorManager::AllocateDescriptorMemory(2 * 1024 * 1024);
		//auto&& handle1 = Graphic::Manager::DescriptorManager::AllocateDescriptorMemory(4 * 1024 * 1024);
		////auto&& handle2 = Graphic::Manager::DescriptorManager::AllocateDescriptorMemory(2 * 1024 * 1024);
		//auto&& handle11 = Graphic::Manager::DescriptorManager::ReallocateDescriptorMemory(handle1, 5 * 1024 * 1024);
		//auto&& handle12 = Graphic::Manager::DescriptorManager::ReallocateDescriptorMemory(handle11, 7 * 1024 * 1024);

		//std::vector<uint8_t> data(10, 5);
		//Graphic::Manager::DescriptorManager::WriteToHostDescriptorMemory(handle0, data.data(), 4, data.size());
		//auto&& offset = handle0.Offset();
		//auto&& size = handle0.Size();

		////Graphic::Manager::DescriptorManager::FreeDescriptorMemory(handle2);
		//Graphic::Manager::DescriptorManager::FreeDescriptorMemory(handle0);
		////Graphic::Manager::DescriptorManager::FreeDescriptorMemory(handle1);
		//Graphic::Manager::DescriptorManager::FreeDescriptorMemory(handle12);

		shaderLoadHandle.SharedFuture().wait();
		auto&& shader = shaderLoadHandle.Asset<Graphic::Rendering::Shader>();
		Graphic::Manager::DescriptorManager::ToAligned(0);
		auto&& material = Graphic::Rendering::Material(shader);
		material.SetUniformBuffer(Utility::InternedString("sampler2d"), &uniformBuffer, 0);
		material.SetUniformBuffer(Utility::InternedString("matrixData"), &uniformBuffer, 0);
		material.SetUniformBuffer(Utility::InternedString("matrixData"), &uniformBuffer, 1);
		material.SetUniformBuffer(Utility::InternedString("sampler2dArray"), &uniformBuffer, 0);
		auto&& ub = material.GetUniformBuffer(Utility::InternedString("sampler2dArray"), 0);
		material.SetUniformBuffer(Utility::InternedString("cubes"), &uniformBuffer, 0);
		material.SetUniformBuffer(Utility::InternedString("cubes"), &uniformBuffer, 15);
		material.SetUniformBuffer(Utility::InternedString("cubes"), &uniformBuffer, 31);

		auto&& dirtyHandles = Graphic::Manager::DescriptorManager::MergeAndClearDirtyHandles();
		Graphic::Manager::DescriptorManager::CopyHostDirtyDataToCachedBuffer(dirtyHandles);
	}

	auto&& currentFrame = _frameResources[_curFrameIndex];
	auto&& currentImage = _imageResources[_curImageIndex];

	_commandPool->Reset();
	_commandBuffer->BeginRecord(vk::CommandBufferUsageFlagBits::eOneTimeSubmit);
	Graphic::Command::Barrier barrier{};
	if (assetLoadHandle.IsCompleted())
	{
		barrier.AddImageMemoryBarrier(
			*currentImage.image,
			vk::PipelineStageFlagBits2::eNone,
			vk::AccessFlagBits2::eNone,
			vk::PipelineStageFlagBits2::eBlit,
			vk::AccessFlagBits2::eTransferWrite,
			vk::ImageLayout::eUndefined,
			vk::ImageLayout::eTransferDstOptimal,
			vk::ImageAspectFlagBits::eColor
		);
		_commandBuffer->AddPipelineBarrier(barrier);
		_commandBuffer->Blit(
			assetLoadHandle.Asset<Asset::Texture2D>().Image(), vk::ImageLayout::eTransferSrcOptimal,
			*currentImage.image, vk::ImageLayout::eTransferDstOptimal,
			vk::ImageAspectFlagBits::eColor,
			vk::Filter::eLinear
		);
		barrier.ClearImageMemoryBarriers();
		barrier.AddImageMemoryBarrier(
			*currentImage.image,
			vk::PipelineStageFlagBits2::eBlit,
			vk::AccessFlagBits2::eTransferWrite,
			vk::PipelineStageFlagBits2::eNone,
			vk::AccessFlagBits2::eNone,
			vk::ImageLayout::eTransferDstOptimal,
			vk::ImageLayout::ePresentSrcKHR,
			vk::ImageAspectFlagBits::eColor
		);
		_commandBuffer->AddPipelineBarrier(barrier);
	}
	else
	{
		barrier.AddImageMemoryBarrier(
			*currentImage.image,
			vk::PipelineStageFlagBits2::eNone,
			vk::AccessFlagBits2::eNone,
			vk::PipelineStageFlagBits2::eClear,
			vk::AccessFlagBits2::eTransferWrite,
			vk::ImageLayout::eUndefined,
			vk::ImageLayout::eTransferDstOptimal,
			vk::ImageAspectFlagBits::eColor
		);
		_commandBuffer->AddPipelineBarrier(barrier);
		_commandBuffer->ClearColorImage<float>(*currentImage.image, vk::ImageLayout::eTransferDstOptimal, { 1.0f, 1.0f, 1.0f, 1.0f });
		barrier.ClearImageMemoryBarriers();
		barrier.AddImageMemoryBarrier(
			*currentImage.image,
			vk::PipelineStageFlagBits2::eClear,
			vk::AccessFlagBits2::eTransferWrite,
			vk::PipelineStageFlagBits2::eNone,
			vk::AccessFlagBits2::eNone,
			vk::ImageLayout::eTransferDstOptimal,
			vk::ImageLayout::ePresentSrcKHR,
			vk::ImageAspectFlagBits::eColor
		);
		_commandBuffer->AddPipelineBarrier(barrier);
	}
	_commandBuffer->EndRecord();

	_commandPool->Queue().ImmediateIndividualSubmit(
		{
			{{currentFrame.acquireSemaphore, vk::PipelineStageFlagBits2::eClear}},
			{_commandBuffer},
			{{currentImage.transferSemaphore, vk::PipelineStageFlagBits2::eNone}}
		},
		*_transferFence
	);

	vk::PresentInfoKHR vkPresentInfo{};
	vkPresentInfo.swapchainCount = 1;
	vkPresentInfo.pSwapchains = &_vkSwapchain;
	vkPresentInfo.pImageIndices = &_curImageIndex;
	vkPresentInfo.waitSemaphoreCount = 1;
	vkPresentInfo.pWaitSemaphores = &currentImage.transferSemaphore->VkHandle();

	_commandPool->Queue().VkHandle().presentKHR(vkPresentInfo);

	while (_transferFence->Status() == vk::Result::eNotReady) Utility::ThisFiber::yield();
	_transferFence->Reset();

	_curFrameIndex = (_curFrameIndex + 1) % 3;
}

void AirEngine::Runtime::Core::FrontEnd::Window::OnSetVulkanHandle()
{
	_qVulkanInstance.setVkInstance(Manager::GraphicDeviceManager::Instance());
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
				vk::Format(_vkbSwapchain.image_format),
				vk::Extent2D(_vkbSwapchain.extent),
				vk::ImageUsageFlags(_vkbSwapchain.image_usage_flags)
			),
			new Graphic::Command::Semaphore()
		);
	}

	_commandPool = new Graphic::Command::CommandPool(Utility::InternedString("PresentQueue"), vk::CommandPoolCreateFlagBits::eTransient);
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
