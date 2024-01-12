#include "FrameBuffer.hpp"
#include "AirEngine/Runtime/Graphic/Manager/DeviceManager.hpp"
#include "AirEngine/Runtime/Graphic/Instance/ImageView.hpp"
#include "AirEngine/Runtime/Graphic/Instance/Image.hpp"
#include "AirEngine/Runtime/Graphic/Instance/RenderPassBase.hpp"
#include <algorithm>

AirEngine::Runtime::Graphic::Instance::FrameBuffer::FrameBuffer()
	: _vkFrameBuffer()
	, _attachments()
	, _extent2D()
	, _renderPass()
{
}

AirEngine::Runtime::Graphic::Instance::FrameBuffer::FrameBuffer(const FrameBufferBuilder& builder)
	: _vkFrameBuffer()
	, _attachments()
	, _extent2D()
	, _renderPass()
{
	if (builder._renderPass == nullptr) qFatal("Render pass must not be null.");

	if (builder._renderPass->Info().AttachmentInfos().size() != builder._attachmentMap.size()) qFatal("Render pass do not contains this attachment.");

	if (std::find_if(builder._vkImageViews.begin(), builder._vkImageViews.end(), [](const auto& x)->bool { return x == vk::ImageView(); }) != builder._vkImageViews.end()) qFatal("Frame buffer should set all attachments.");

	vk::FramebufferCreateInfo createInfo(vk::FramebufferCreateFlags(0), builder._renderPass->VkHandle(), builder._vkImageViews.size(), builder._vkImageViews.data(), builder._extent2D.width, builder._extent2D.height, 1);
	auto&& vkFrameBuffer = Manager::DeviceManager::Device().createFramebuffer(createInfo);

	_attachments = builder._attachmentMap;
	_extent2D = builder._extent2D;
	_vkFrameBuffer = vkFrameBuffer;
	_renderPass = builder._renderPass;
}

AirEngine::Runtime::Graphic::Instance::FrameBuffer::~FrameBuffer()
{
	Manager::DeviceManager::Device().destroyFramebuffer(_vkFrameBuffer);
}

AirEngine::Runtime::Graphic::Instance::FrameBufferBuilder::FrameBufferBuilder()
	: _attachmentMap()
	, _vkImageViews()
	, _extent2D()
	, _renderPass()
{
}

AirEngine::Runtime::Graphic::Instance::FrameBufferBuilder& AirEngine::Runtime::Graphic::Instance::FrameBufferBuilder::SetRenderPass(RenderPassBase* renderPass)
{
	if (renderPass == nullptr) qFatal("Render pass must not be null.");

	_attachmentMap.clear();
	_vkImageViews.clear();
	_vkImageViews.resize(renderPass->Info().AttachmentInfos().size());

	_renderPass = renderPass;

	return *this;
}

AirEngine::Runtime::Graphic::Instance::FrameBufferBuilder& AirEngine::Runtime::Graphic::Instance::FrameBufferBuilder::SetAttachment(Utility::InternedString name, Instance::ImageView* imageView)
{
	if (_renderPass == nullptr) qFatal("Render pass must not be null.");
	if (imageView == nullptr) qFatal("Attachment must not be null.");
	if (imageView->ImageViewType() != vk::ImageViewType::e2D) qFatal("Attachment must be 2d.");
	if (_attachmentMap.size() == 0)
	{
		_extent2D = imageView->Image()->Extent2D();
	}
	else if (_extent2D != imageView->Image()->Extent2D()) qFatal("Attachment must have the same extent.");

	auto iter = _renderPass->Info().AttachmentInfos().find(name);
	if(iter == _renderPass->Info().AttachmentInfos().end()) qFatal("Render pass do not contains this attachment.");

	_attachmentMap.emplace(name, imageView);
	_vkImageViews.at(iter->second.attachmentIndex) = imageView->VkHandle();

	return *this;
}

AirEngine::Runtime::Graphic::Instance::FrameBuffer* AirEngine::Runtime::Graphic::Instance::FrameBufferBuilder::Build()
{
	if (_renderPass == nullptr) qFatal("Render pass must not be null.");

	if(_renderPass->Info().AttachmentInfos().size() != _attachmentMap.size()) qFatal("Render pass do not contains this attachment.");
	
	if(std::find_if(_vkImageViews.begin(), _vkImageViews.end(), [](const auto& x)->bool { return x == vk::ImageView(); }) != _vkImageViews.end()) qFatal("Frame buffer should set all attachments.");

	vk::FramebufferCreateInfo createInfo(vk::FramebufferCreateFlags(0), _renderPass->VkHandle(), _vkImageViews.size(), _vkImageViews.data(), _extent2D.width, _extent2D.height, 1);
	auto&& vkFrameBuffer = Manager::DeviceManager::Device().createFramebuffer(createInfo);

	auto&& newFrameBuffer = new FrameBuffer();
	newFrameBuffer->_attachments = _attachmentMap;
	newFrameBuffer->_extent2D = _extent2D;
	newFrameBuffer->_vkFrameBuffer = vkFrameBuffer;
	newFrameBuffer->_renderPass = _renderPass;

	return newFrameBuffer;
}
