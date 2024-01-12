﻿#include "Window.hpp"
#include "AirEngine/Runtime/Graphic/Manager/DeviceManager.hpp"
#include <vulkan/vulkan.hpp>
#include "AirEngine/Runtime/Graphic/Instance/Image.hpp"
#include "AirEngine/Runtime/Graphic/Command/Semaphore.hpp"
#include "AirEngine/Runtime/Graphic/Command/Fence.hpp"
#include "AirEngine/Runtime/Graphic/Command/Barrier.hpp"
#include "AirEngine/Runtime/Graphic/Command/CommandPool.hpp"
#include "AirEngine/Runtime/Graphic/Command/CommandBuffer.hpp"
#include "AirEngine/Runtime/Graphic/Instance/Queue.hpp"
#include "AirEngine/Runtime/Graphic/Asset/Loader/Texture2DLoader.hpp"
#include "AirEngine/Runtime/Graphic/Asset/Texture2D.hpp"
#include "AirEngine/Runtime/Graphic/Asset/Mesh.hpp"
#include "AirEngine/Runtime/Asset/Manager/AssetManager.hpp"
#include "AirEngine/Runtime/Graphic/Manager/RenderPassManager.hpp"
#include "AirEngine/Runtime/Graphic/Manager/DescriptorManager.hpp"
#include "AirEngine/Runtime/Graphic/Instance/RenderPassBase.hpp"
#include "AirEngine/Runtime/Graphic/Instance/Buffer.hpp"
#include "AirEngine/Runtime/Graphic/Rendering/Shader.hpp"
#include "AirEngine/Runtime/Graphic/Rendering/Material.hpp"
#include "AirEngine/Runtime/Graphic/Instance/UniformBuffer.hpp"
#include "AirEngine/Runtime/Graphic/Instance/ImageSampler.hpp"
#include "AirEngine/Runtime/Graphic/Instance/FrameBuffer.hpp"
#include "AirEngine/Runtime/Graphic/Manager/DescriptorManager.hpp"
#include "AirEngine/Runtime/Graphic/Manager/ImageSamplerManager.hpp"
#include <QPlatformSurfaceEvent>

void AirEngine::Runtime::FrontEnd::Window::OnCreateSurface()
{
	//setMinimumSize({ 1600, 900 });
	//setMaximumSize({ 1600, 900 });
	resize(1600, 900);
	setSurfaceType(QSurface::VulkanSurface);
	show();

	_qVulkanInstance.setVkInstance(Graphic::Manager::DeviceManager::Instance());
	bool qResult = _qVulkanInstance.create();
	if (!qResult) qFatal("Create vulkan instance failed.");

	setVulkanInstance(&_qVulkanInstance);
	_vkSurface = _qVulkanInstance.surfaceForWindow(this);
}

void AirEngine::Runtime::FrontEnd::Window::OnCreateSwapchain()
{
	RecreateVulkanSwapchain();
}

bool AirEngine::Runtime::FrontEnd::Window::AcquireImage()
{
	auto&& realWindowExtent = size() * devicePixelRatio();
	if (realWindowExtent.width() != _vkbSwapchain.extent.width || realWindowExtent.height() != _vkbSwapchain.extent.height)
	{
		RecreateVulkanSwapchain();
	}

	auto&& currentFrame = _frameResources[_curFrameIndex];

	while (currentFrame.acquireFence->Status() == vk::Result::eNotReady) std::this_thread::yield();

	currentFrame.acquireFence->Reset();
	vk::ResultValue<uint32_t> acquireResult(vk::Result::eSuccess, 0);
	try
	{
		acquireResult = Graphic::Manager::DeviceManager::Device().acquireNextImageKHR(
			_vkSwapchain,
			std::numeric_limits<uint64_t>::max(),
			currentFrame.acquireSemaphore->VkHandle(),
			currentFrame.acquireFence->VkHandle()
		);
	}
	catch (vk::OutOfDateKHRError e)
	{
		RecreateVulkanSwapchain();
		return false;
	}
	if (acquireResult.result == vk::Result::eSuccess || acquireResult.result == vk::Result::eSuboptimalKHR)
	{
		_curImageIndex = acquireResult.value;
		return true;
	}
	else
	{
		qFatal("Fail to acquire next image.");
		return false;
	}
}

static bool isLoaded = false;
static AirEngine::Runtime::Asset::Loader::LoadHandle sampledImageLoadHandle{};
static AirEngine::Runtime::Asset::Loader::LoadHandle ndcFullScreenMeshLoadHandle{};
static AirEngine::Runtime::Asset::Loader::LoadHandle presentShaderLoadHandle{};

bool AirEngine::Runtime::FrontEnd::Window::Present()
{
	if (!isLoaded)
	{
		isLoaded = true;

		auto&& sampler = new Graphic::Instance::ImageSampler(vk::Filter::eNearest, vk::Filter::eNearest, vk::SamplerMipmapMode::eLinear, vk::SamplerAddressMode::eRepeat, vk::SamplerAddressMode::eRepeat, vk::SamplerAddressMode::eRepeat, 0, 1);
		auto&& builtinSampler = Graphic::Manager::ImageSamplerManager::ImageSampler(vk::Filter::eNearest, vk::SamplerMipmapMode::eNearest, vk::SamplerAddressMode::eRepeat, 0, 10);

		sampledImageLoadHandle = Asset::Manager::AssetManager::LoadAsset("..\\../Resources\\Texture/WorkShop_Equirectangular.texture2d");
		ndcFullScreenMeshLoadHandle = Asset::Manager::AssetManager::LoadAsset("..\\../Resources\\Mesh/NdcFullScreen.mesh");
		presentShaderLoadHandle = Asset::Manager::AssetManager::LoadAsset("..\\../Resources\\Shader/Present.shader");

		{
			auto presentRenderPass = Graphic::Manager::RenderPassManager::LoadRenderPass<PresentRenderPass>();
			auto attachmentImage = std::make_unique<Graphic::Instance::Image>(
				vk::Format::eR8G8B8A8Srgb,
				vk::Extent3D{ 256, 256, 1 },
				vk::ImageType::e2D,
				1, 1,
				vk::ImageUsageFlagBits::eColorAttachment,
				vk::MemoryPropertyFlagBits::eDeviceLocal
			);
			auto swapchainAttachmentName = Utility::InternedString("SwapchainAttachment");
			auto colorAttachment = attachmentImage->AddImageView(swapchainAttachmentName, vk::ImageViewType::e2D, vk::ImageLayout::eAttachmentOptimal, vk::ImageAspectFlagBits::eColor, 0, 1, 0, 1);
			auto frameBuffer = Graphic::Instance::FrameBufferBuilder()
				.SetRenderPass(presentRenderPass)
				.SetAttachment(swapchainAttachmentName, colorAttachment)
				.Build();
		}


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
		sampledImageLoadHandle.SharedFuture().wait();
		auto&& sampledImage = sampledImageLoadHandle.Asset<Graphic::Asset::Texture2D>();
		ndcFullScreenMeshLoadHandle.SharedFuture().wait();
		auto&& ndcFullScreenMesh = ndcFullScreenMeshLoadHandle.Asset<Graphic::Asset::Mesh>();
		presentShaderLoadHandle.SharedFuture().wait();
		auto&& presentShader = presentShaderLoadHandle.Asset<Graphic::Rendering::Shader>();

		auto&& presentMaterial = Graphic::Rendering::Material(presentShader);
		presentMaterial.SetImageSampler(Utility::InternedString("sourceAttachmentSampler"), Graphic::Manager::ImageSamplerManager::ImageSampler(vk::Filter::eLinear, vk::SamplerMipmapMode::eLinear, vk::SamplerAddressMode::eMirroredRepeat, 0, 1));
		presentMaterial.SetSampledImage(Utility::InternedString("sourceAttachment"), sampledImage.ImageView(Utility::InternedString("Sampled")));

		//presentMaterial.SetUniformBuffer(Utility::InternedString("sampler2d"), &uniformBuffer, 0);
		//presentMaterial.SetUniformBuffer(Utility::InternedString("matrixData"), &uniformBuffer, 0);
		//presentMaterial.SetUniformBuffer(Utility::InternedString("matrixData"), &uniformBuffer, 1);
		//presentMaterial.SetUniformBuffer(Utility::InternedString("sampler2dArray"), &uniformBuffer, 0);
		//auto&& ub = presentMaterial.GetUniformBuffer(Utility::InternedString("sampler2dArray"), 0);
		//presentMaterial.SetUniformBuffer(Utility::InternedString("cubes"), &uniformBuffer, 0);
		//presentMaterial.SetUniformBuffer(Utility::InternedString("cubes"), &uniformBuffer, 15);
		//presentMaterial.SetUniformBuffer(Utility::InternedString("cubes"), &uniformSetUniformBufferBuffer, 31);
		//presentMaterial.(Utility::InternedString("cubes"), nullptr, 0);

		//auto&& dirtyHandles = Graphic::Manager::DescriptorManager::MergeAndClearDirtyHandles();
		//Graphic::Manager::DescriptorManager::CopyHostDirtyDataToCachedBuffer(dirtyHandles);
	}

	auto&& currentFrame = _frameResources[_curFrameIndex];
	auto&& currentImage = _imageResources[_curImageIndex];

	_commandPool->Reset();
	_commandBuffer->BeginRecord(vk::CommandBufferUsageFlagBits::eOneTimeSubmit);
	Graphic::Command::Barrier barrier{};
	//if (sampledImageLoadHandle.IsCompleted())
	//{
	//	barrier.AddImageMemoryBarrier(
	//		*currentImage.image,
	//		vk::PipelineStageFlagBits2::eNone,
	//		vk::AccessFlagBits2::eNone,
	//		vk::PipelineStageFlagBits2::eBlit,
	//		vk::AccessFlagBits2::eTransferWrite,
	//		vk::ImageLayout::eUndefined,
	//		vk::ImageLayout::eTransferDstOptimal,
	//		vk::ImageAspectFlagBits::eColor
	//	);
	//	_commandBuffer->AddPipelineBarrier(barrier);
	//	_commandBuffer->Blit(
	//		sampledImageLoadHandle.Asset<Graphic::Asset::Texture2D>(), vk::ImageLayout::eTransferSrcOptimal,
	//		*currentImage.image, vk::ImageLayout::eTransferDstOptimal,
	//		vk::ImageAspectFlagBits::eColor,
	//		vk::Filter::eLinear
	//	);
	//	barrier.ClearImageMemoryBarriers();
	//	barrier.AddImageMemoryBarrier(
	//		*currentImage.image,
	//		vk::PipelineStageFlagBits2::eBlit,
	//		vk::AccessFlagBits2::eTransferWrite,
	//		vk::PipelineStageFlagBits2::eNone,
	//		vk::AccessFlagBits2::eNone,
	//		vk::ImageLayout::eTransferDstOptimal,
	//		vk::ImageLayout::ePresentSrcKHR,
	//		vk::ImageAspectFlagBits::eColor
	//	);
	//	_commandBuffer->AddPipelineBarrier(barrier);
	//}
	//else
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

	_transferFence->Reset();
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

	_qVulkanInstance.presentAboutToBeQueued(this);
	vk::Result presentResult{};
	try
	{
		presentResult = _commandPool->Queue().VkHandle().presentKHR(vkPresentInfo);
	}
	catch (vk::OutOfDateKHRError e)
	{
		RecreateVulkanSwapchain();
		return false;
	}
	if (presentResult == vk::Result::eSuccess || presentResult == vk::Result::eSuboptimalKHR)
	{
		while (_transferFence->Status() == vk::Result::eNotReady) std::this_thread::yield();
	}
	else
	{
		qFatal("Fail to present.");
		return false;
	}
	_qVulkanInstance.presentQueued(this);

	_curFrameIndex = (_curFrameIndex + 1) % 3;

	return true;
}

void AirEngine::Runtime::FrontEnd::Window::RecreateVulkanSwapchain()
{
	vkb::SwapchainBuilder swapchainBuilder(Graphic::Manager::DeviceManager::VkbDevice());
	swapchainBuilder = swapchainBuilder.set_desired_format({ VK_FORMAT_B8G8R8A8_SRGB, VK_COLOR_SPACE_SRGB_NONLINEAR_KHR })
		.set_desired_present_mode(VK_PRESENT_MODE_FIFO_KHR)
		.set_pre_transform_flags(VK_SURFACE_TRANSFORM_IDENTITY_BIT_KHR)
		.set_composite_alpha_flags(VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR)
		.set_clipped(false)
		.set_image_array_layer_count(1)
		.set_required_min_image_count(3)
		.set_image_usage_flags(VkImageUsageFlagBits::VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT | VkImageUsageFlagBits::VK_IMAGE_USAGE_TRANSFER_DST_BIT)
		.set_old_swapchain(_vkbSwapchain);
	auto swapchainResult = swapchainBuilder.build();
	
	if (_vkSwapchain)
	{
		DestroyVulkanSwapchain();
	}
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

	_transferFence = new Graphic::Command::Fence(true);
}

void AirEngine::Runtime::FrontEnd::Window::DestroyVulkanSwapchain()
{
	if (!_vkSwapchain) return;

	Graphic::Manager::DeviceManager::DeviceManager().Device().waitIdle();

	_transferFence->Wait();
	delete _transferFence;
	delete _commandPool;

	for (auto& frameResource : _frameResources)
	{
		frameResource.acquireFence->Wait();
		delete frameResource.acquireFence;
		delete frameResource.acquireSemaphore;
	}
	_frameResources.clear();

	for (auto& imageResource : _imageResources)
	{
		delete imageResource.transferSemaphore;
		delete imageResource.image;
	}
	_imageResources.clear();

	vkb::destroy_swapchain(_vkbSwapchain); 
	_vkSwapchain = vk::SwapchainKHR();
	_vkbSwapchain.swapchain = _vkSwapchain;
}

void AirEngine::Runtime::FrontEnd::Window::OnFinishRender()
{
	_beginPresentCondition.Awake();

	//Present in qt thread

	_endPresentCondition.Wait();
	_endPresentCondition.Reset();
}

void AirEngine::Runtime::FrontEnd::Window::OnStartRender()
{
}

void AirEngine::Runtime::FrontEnd::Window::exposeEvent(QExposeEvent* e)
{
	if (isExposed()) 
	{
		if (!_vkSwapchain)
		{
			RecreateVulkanSwapchain();
		}

		requestUpdate();
	}
	else 
	{
		DestroyVulkanSwapchain();
	}
}

void AirEngine::Runtime::FrontEnd::Window::resizeEvent(QResizeEvent*)
{
	// Nothing to do here - recreating the swapchain is handled when building the next frame.
}

bool AirEngine::Runtime::FrontEnd::Window::event(QEvent* e)
{
	switch (e->type()) 
	{
		case QEvent::UpdateRequest:
		{
			if (!_vkSwapchain) break;

			const bool isAcquireSuccessed = AcquireImage();
			if (!isAcquireSuccessed)
			{
				requestUpdate();
				break;
			}

			ReadyToRender();

			//Render in main loop

			_beginPresentCondition.Wait();
			_beginPresentCondition.Reset();

			const bool isPresentSuccessed = Present();

			_endPresentCondition.Awake();

			if (!isPresentSuccessed)
			{
				requestUpdate();
				break;
			}

			requestUpdate();

			break;
		}
			// The swapchain must be destroyed before the surface as per spec. This is
			// not ideal for us because the surface is managed by the QPlatformWindow
			// which may be gone already when the unexpose comes, making the validation
			// layer scream. The solution is to listen to the PlatformSurface events.
		case QEvent::PlatformSurface:
		{
			if (static_cast<QPlatformSurfaceEvent*>(e)->surfaceEventType() == QPlatformSurfaceEvent::SurfaceAboutToBeDestroyed)
			{
				DestroyVulkanSwapchain();
			}
			break;
		}
		default:
			break;
	}

	return QWindow::event(e);
}

AirEngine::Runtime::FrontEnd::Window::Window()
	: QWindow()
	, WindowFrontEndBase()
	, _endPresentCondition()
	, _beginPresentCondition()
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

REGISTRATION
{
	DECLARE_TYPE(AirEngine::Runtime::FrontEnd::PresentRenderPass*)
}
AirEngine::Runtime::FrontEnd::PresentRenderPass::PresentRenderPass()
	: RenderPassBase(
		RenderPassBase::RenderPassBuilder()
		.SetName("PresentRenderPass")
		.AddColorAttachment(
			"SwapchainAttachment",
			vk::Format::eR8G8B8A8Srgb,
			vk::AttachmentLoadOp::eClear, vk::AttachmentStoreOp::eStore,
			vk::ImageLayout::eColorAttachmentOptimal, vk::ImageLayout::eColorAttachmentOptimal
		)
		.AddDependency(
			"ExternalSubpass", "PresentSubpass",
			vk::PipelineStageFlagBits2::eNone, vk::PipelineStageFlagBits2::eNone,
			vk::AccessFlagBits2::eNone, vk::AccessFlagBits2::eNone
		)
		.AddSubpass(
			RenderPassBase::RenderSubpassBuilder()
			.SetName("PresentSubpass")
			.SetPipelineBindPoint(vk::PipelineBindPoint::eGraphics)
			.AddColorAttachment("SwapchainAttachment", vk::ImageLayout::eColorAttachmentOptimal)
		)
		.AddDependency(
			"PresentSubpass", "ExternalSubpass",
			vk::PipelineStageFlagBits2::eNone, vk::PipelineStageFlagBits2::eNone,
			vk::AccessFlagBits2::eNone, vk::AccessFlagBits2::eNone
		)
	)
{
}
#include "moc_Window.cpp"