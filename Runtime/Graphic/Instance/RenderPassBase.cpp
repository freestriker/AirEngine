#include "RenderPassBase.hpp"
#include "../../Core/Manager/GraphicDeviceManager.hpp"

REGISTRATION
{
	DECLARE_TYPE(AirEngine::Runtime::Graphic::Instance::RenderPassBase*)
	DECLARE_TYPE(AirEngine::Runtime::Graphic::Instance::DummyRenderPass*)
}

AirEngine::Runtime::Graphic::Instance::RenderPassBase::RenderPassBuilder& AirEngine::Runtime::Graphic::Instance::RenderPassBase::RenderPassBuilder::SetName(const std::string& name)
{
	_name = name;
	return *this;
}

AirEngine::Runtime::Graphic::Instance::RenderPassBase::RenderPassBuilder& AirEngine::Runtime::Graphic::Instance::RenderPassBase::RenderPassBuilder::AddColorAttachment(
	const std::string& name,
	vk::Format format,
	vk::AttachmentLoadOp loadOp, vk::AttachmentStoreOp storeOp,
	vk::ImageLayout initialLayout, vk::ImageLayout finalLayout
)
{
	_vkAttachmentDescriptionMap[name] = 
	{
		vk::AttachmentDescriptionFlags(0),
		format,
		vk::SampleCountFlagBits::e1,
		loadOp, storeOp,
		vk::AttachmentLoadOp::eDontCare, vk::AttachmentStoreOp::eDontCare,
		initialLayout, finalLayout
	};
	return *this;
}

AirEngine::Runtime::Graphic::Instance::RenderPassBase::RenderPassBuilder& AirEngine::Runtime::Graphic::Instance::RenderPassBase::RenderPassBuilder::AddSubpass(const RenderSubpassBuilder& renderSubpassBuilder)
{
	_renderSubpassBuilders.emplace_back(renderSubpassBuilder);
	return *this;
}

AirEngine::Runtime::Graphic::Instance::RenderPassBase::RenderPassBuilder& AirEngine::Runtime::Graphic::Instance::RenderPassBase::RenderPassBuilder::AddDependency(
	const std::string& srcSubpass,
	const std::string& dstSubpass,
	vk::PipelineStageFlags2 srcStageMask, vk::PipelineStageFlags2 dstStageMask,
	vk::AccessFlags2 srcAccessMask, vk::AccessFlags2 dstAccessMask,
	vk::DependencyFlags dependencyFlags
)
{
	_srcDstDependencyBarriers.emplace_back
	(
		srcSubpass, dstSubpass, 
		dependencyFlags,
		vk::MemoryBarrier2(
			srcStageMask, srcAccessMask, 
			dstStageMask, dstAccessMask
		)
	);
	return *this;
}

vk::RenderPass AirEngine::Runtime::Graphic::Instance::RenderPassBase::RenderPassBuilder::Build() const
{
	std::map<std::string, uint32_t> attachmentNameToIndexMap{};
	std::vector<vk::AttachmentDescription2> vkAttachmentDescriptions{};
	{
		vkAttachmentDescriptions.reserve(_vkAttachmentDescriptionMap.size());

		uint32_t vkAttachmentDescriptionIndex = 0;
		for (const auto& vkAttachmentDescription : _vkAttachmentDescriptionMap)
		{
			vkAttachmentDescriptions.emplace_back(vkAttachmentDescription.second);
			attachmentNameToIndexMap[vkAttachmentDescription.first] = vkAttachmentDescriptionIndex++;
		}
	}

	std::map<std::string, uint32_t> subpassNameToIndexMap{};
	std::vector<vk::SubpassDescription2> vkSubpassDescriptions{_renderSubpassBuilders.size()};
	std::vector<vk::AttachmentReference2> vkAttachmentReferences{};
	std::vector<uint32_t> preserveAttachmentIndexs{};
	uint32_t totalAttachmentReferenceCount = 0;
	uint32_t totalPreserveAttachmentCount = 0;
	{
		subpassNameToIndexMap["ExternalSubpass"] = VK_SUBPASS_EXTERNAL;
		for (uint32_t subpassIndex = 0; subpassIndex < _renderSubpassBuilders.size(); subpassIndex++)
		{
			const auto& subpassBuilder = _renderSubpassBuilders[subpassIndex];
			auto& vkSubpassDescription = vkSubpassDescriptions[subpassIndex];

			vkSubpassDescription.flags = vk::SubpassDescriptionFlags(0);
			vkSubpassDescription.pipelineBindPoint = vkSubpassDescription.pipelineBindPoint;
			vkSubpassDescription.viewMask = 0;
			vkSubpassDescription.inputAttachmentCount = subpassBuilder._inputReferences.size();
			vkSubpassDescription.pInputAttachments = reinterpret_cast<vk::AttachmentReference2*>(size_t(totalAttachmentReferenceCount));
			totalAttachmentReferenceCount += subpassBuilder._inputReferences.size();
			vkSubpassDescription.colorAttachmentCount = subpassBuilder._coloreReferences.size();
			vkSubpassDescription.pColorAttachments = reinterpret_cast<vk::AttachmentReference2*>(size_t(totalAttachmentReferenceCount));
			totalAttachmentReferenceCount += subpassBuilder._coloreReferences.size();
			vkSubpassDescription.pResolveAttachments = nullptr;
			totalAttachmentReferenceCount += 0;
			if (subpassBuilder._depthReference)
			{
				vkSubpassDescription.pDepthStencilAttachment = reinterpret_cast<vk::AttachmentReference2*>(size_t(totalAttachmentReferenceCount));
				totalAttachmentReferenceCount += 1;
			}
			else
			{
				vkSubpassDescription.pDepthStencilAttachment = nullptr;
			}
			vkSubpassDescription.preserveAttachmentCount = subpassBuilder._preserveReferences.size();
			vkSubpassDescription.pPreserveAttachments = reinterpret_cast<uint32_t*>(size_t(totalPreserveAttachmentCount));
			totalPreserveAttachmentCount += subpassBuilder._preserveReferences.size();

			vkSubpassDescription.pPreserveAttachments = nullptr;
			totalAttachmentReferenceCount += subpassBuilder._inputReferences.size();

			vkSubpassDescription.pNext = nullptr;

			subpassNameToIndexMap[subpassBuilder._name] = subpassIndex;
		}

		vkAttachmentReferences.reserve(totalAttachmentReferenceCount);
		preserveAttachmentIndexs.reserve(totalPreserveAttachmentCount);

		for (uint32_t subpassIndex = 0; subpassIndex < _renderSubpassBuilders.size(); subpassIndex++)
		{
			const auto& subpassBuilder = _renderSubpassBuilders[subpassIndex];
			auto& vkSubpassDescription = vkSubpassDescriptions[subpassIndex];
			
			for (const auto& inputReferencePair: subpassBuilder._inputReferences)
			{
				vkAttachmentReferences.emplace_back(inputReferencePair.second);
				vkAttachmentReferences.back().attachment = attachmentNameToIndexMap[inputReferencePair.first];
			}
			vkSubpassDescription.pInputAttachments = vkAttachmentReferences.data() + reinterpret_cast<size_t>(vkSubpassDescription.pInputAttachments);

			for (const auto& colorReferencePair: subpassBuilder._coloreReferences)
			{
				vkAttachmentReferences.emplace_back(colorReferencePair.second);
				vkAttachmentReferences.back().attachment = attachmentNameToIndexMap[colorReferencePair.first];
			}
			vkSubpassDescription.pColorAttachments = vkAttachmentReferences.data() + reinterpret_cast<size_t>(vkSubpassDescription.pColorAttachments);

			if (subpassBuilder._depthReference)
			{
				vkAttachmentReferences.emplace_back(subpassBuilder._depthReference.value().second);
				vkAttachmentReferences.back().attachment = attachmentNameToIndexMap[subpassBuilder._depthReference.value().first];
				vkSubpassDescription.pDepthStencilAttachment = vkAttachmentReferences.data() + reinterpret_cast<size_t>(vkSubpassDescription.pDepthStencilAttachment);
			}
			else
			{
				vkSubpassDescription.pDepthStencilAttachment = nullptr;
			}

			for (const auto& preserveReference : subpassBuilder._preserveReferences)
			{
				preserveAttachmentIndexs.emplace_back(attachmentNameToIndexMap[preserveReference]);
			}
			vkSubpassDescription.pPreserveAttachments = preserveAttachmentIndexs.data() + reinterpret_cast<size_t>(vkSubpassDescription.pPreserveAttachments);
		}
	}

	std::vector<vk::SubpassDependency2> vkSubpassDependencys{_srcDstDependencyBarriers.size(), vk::SubpassDependency2{}};
	std::vector<vk::MemoryBarrier2> vkMemoryBarriers{_srcDstDependencyBarriers.size(), vk::MemoryBarrier2{}};
	{
		for (uint32_t dependencyIndex = 0; dependencyIndex < _srcDstDependencyBarriers.size(); dependencyIndex++)
		{
			const auto& tuple = _srcDstDependencyBarriers[dependencyIndex];
			auto& vkSubpassDependency = vkSubpassDependencys[dependencyIndex];
			auto& vkMemoryBarrier = std::get<3>(tuple);

			vkSubpassDependency.srcSubpass = subpassNameToIndexMap[std::get<0>(tuple)];
			vkSubpassDependency.dstSubpass = subpassNameToIndexMap[std::get<1>(tuple)];
			vkSubpassDependency.srcStageMask = vk::PipelineStageFlags(0);
			vkSubpassDependency.dstStageMask = vk::PipelineStageFlags(0);
			vkSubpassDependency.srcAccessMask = vk::AccessFlags(0);
			vkSubpassDependency.dstAccessMask = vk::AccessFlags(0);
			vkSubpassDependency.dependencyFlags = std::get<2>(tuple);
			vkSubpassDependency.viewOffset = 0;
			vkSubpassDependency.pNext = &vkMemoryBarrier;
		}
	}

	vk::RenderPassCreateInfo2 vkRenderPassCreateInfo{};
	vkRenderPassCreateInfo.flags = vk::RenderPassCreateFlags(0);
	vkRenderPassCreateInfo.attachmentCount = vkAttachmentDescriptions.size();
	vkRenderPassCreateInfo.pAttachments = vkAttachmentDescriptions.data();
	vkRenderPassCreateInfo.subpassCount = vkSubpassDescriptions.size();
	vkRenderPassCreateInfo.pSubpasses = vkSubpassDescriptions.data();
	vkRenderPassCreateInfo.dependencyCount = vkSubpassDependencys.size();
	vkRenderPassCreateInfo.pDependencies = vkSubpassDependencys.data();
	vkRenderPassCreateInfo.correlatedViewMaskCount = 0;
	vkRenderPassCreateInfo.pCorrelatedViewMasks = nullptr;
	vkRenderPassCreateInfo.pNext = nullptr;

	return vk::Device(Core::Manager::GraphicDeviceManager::VkDevice()).createRenderPass2(vkRenderPassCreateInfo);
}

AirEngine::Runtime::Graphic::Instance::RenderPassBase::RenderPassInfo AirEngine::Runtime::Graphic::Instance::RenderPassBase::RenderPassBuilder::BuildInfo() const
{
	std::map<std::string, uint32_t> attachmentNameToIndexMap{};
	std::vector<vk::AttachmentDescription2> vkAttachmentDescriptions{};
	{
		vkAttachmentDescriptions.reserve(_vkAttachmentDescriptionMap.size());

		uint32_t vkAttachmentDescriptionIndex = 0;
		for (const auto& vkAttachmentDescription : _vkAttachmentDescriptionMap)
		{
			vkAttachmentDescriptions.emplace_back(vkAttachmentDescription.second);
			attachmentNameToIndexMap[vkAttachmentDescription.first] = vkAttachmentDescriptionIndex++;
		}
	}

	RenderPassInfo renderPassInfo{};
	renderPassInfo._name = Utility::InternedString(_name);

	std::unordered_map<Utility::InternedString, SubPassInfo>& subPassInfos = renderPassInfo._subPassInfos;
	for (uint32_t subpassIndex = 0; subpassIndex < _renderSubpassBuilders.size(); subpassIndex++)
	{
		const auto& subpassBuilder = _renderSubpassBuilders[subpassIndex];

		SubPassInfo subPassInfo{};
		subPassInfo._name = Utility::InternedString(subpassBuilder._name);

		for (const auto& pair : subpassBuilder._coloreReferences)
		{
			AttachmentInfo attachmentInfo{};
			attachmentInfo.name = Utility::InternedString(pair.first);
			attachmentInfo.location = attachmentNameToIndexMap.at(pair.first);
			attachmentInfo.format = vkAttachmentDescriptions.at(attachmentNameToIndexMap.at(pair.first)).format;
			attachmentInfo.layout = pair.second.layout;
			attachmentInfo.type = AttachmentType::COLOR;

			subPassInfo._locationToIndexMap.emplace(attachmentInfo.location, uint32_t(subPassInfo._attachments.size()));
			subPassInfo._nameToIndexMap.emplace(attachmentInfo.name, uint32_t(subPassInfo._attachments.size()));

			subPassInfo._attachments.emplace_back(attachmentInfo);
		}
		if (subpassBuilder._depthReference)
		{
			const auto& pair = subpassBuilder._depthReference.value();

			AttachmentInfo attachmentInfo{};
			attachmentInfo.name = Utility::InternedString(pair.first);
			attachmentInfo.location = attachmentNameToIndexMap.at(pair.first);
			attachmentInfo.format = vkAttachmentDescriptions.at(attachmentNameToIndexMap.at(pair.first)).format;
			attachmentInfo.layout = pair.second.layout;
			attachmentInfo.type = AttachmentType::DEPTH;

			subPassInfo._locationToIndexMap.emplace(attachmentInfo.location, uint32_t(subPassInfo._attachments.size()));
			subPassInfo._nameToIndexMap.emplace(attachmentInfo.name, uint32_t(subPassInfo._attachments.size()));

			subPassInfo._attachments.emplace_back(attachmentInfo);

		}
		for (const auto& pair : subpassBuilder._inputReferences)
		{
			AttachmentInfo attachmentInfo{};
			attachmentInfo.name = Utility::InternedString(pair.first);
			attachmentInfo.location = attachmentNameToIndexMap.at(pair.first);
			attachmentInfo.format = vkAttachmentDescriptions.at(attachmentNameToIndexMap.at(pair.first)).format;
			attachmentInfo.layout = pair.second.layout;
			attachmentInfo.type = AttachmentType::INPUT;

			subPassInfo._locationToIndexMap.emplace(attachmentInfo.location, uint32_t(subPassInfo._attachments.size()));
			subPassInfo._nameToIndexMap.emplace(attachmentInfo.name, uint32_t(subPassInfo._attachments.size()));

			subPassInfo._attachments.emplace_back(attachmentInfo);
		}

		subPassInfos.emplace(subPassInfo._name, subPassInfo);
	}

	return renderPassInfo;
}

AirEngine::Runtime::Graphic::Instance::RenderPassBase::RenderSubpassBuilder& AirEngine::Runtime::Graphic::Instance::RenderPassBase::RenderSubpassBuilder::SetName(const std::string& name)
{
	_name = name;
	return *this;
}

AirEngine::Runtime::Graphic::Instance::RenderPassBase::RenderSubpassBuilder& AirEngine::Runtime::Graphic::Instance::RenderPassBase::RenderSubpassBuilder::SetPipelineBindPoint(vk::PipelineBindPoint pipelineBindPoint)
{
	_pipelineBindPoint = pipelineBindPoint;
	return *this;
}

AirEngine::Runtime::Graphic::Instance::RenderPassBase::RenderSubpassBuilder& AirEngine::Runtime::Graphic::Instance::RenderPassBase::RenderSubpassBuilder::AddColorAttachment(const std::string& name, vk::ImageLayout layout)
{
	_coloreReferences[name] =
	{
		uint32_t(-1),
		layout, 
		vk::ImageAspectFlags(0)
	};
	return *this;
}

AirEngine::Runtime::Graphic::Instance::RenderPassBase::RenderSubpassBuilder& AirEngine::Runtime::Graphic::Instance::RenderPassBase::RenderSubpassBuilder::AddDepthAttachment(const std::string& name, vk::ImageLayout layout)
{
	_depthReference =
	{
		name,
		{
			uint32_t(-1),
			layout,
			vk::ImageAspectFlags(0)
		}
	};
	return *this;
}

AirEngine::Runtime::Graphic::Instance::RenderPassBase::RenderSubpassBuilder& AirEngine::Runtime::Graphic::Instance::RenderPassBase::RenderSubpassBuilder::AddPreserveAttachment(const std::string& name)
{
	_preserveReferences.emplace_back(name);
	return *this;
}

AirEngine::Runtime::Graphic::Instance::RenderPassBase::RenderSubpassBuilder& AirEngine::Runtime::Graphic::Instance::RenderPassBase::RenderSubpassBuilder::AddInputAttachment(const std::string& name, vk::ImageLayout layout, vk::ImageAspectFlags aspectMask)
{
	_inputReferences[name] =
	{
		uint32_t(-1),
		layout,
		aspectMask
	};
	return *this;
}

AirEngine::Runtime::Graphic::Instance::RenderPassBase::RenderPassBase(const RenderPassBuilder& renderPassBuilder)
	: _vkRenderPass(renderPassBuilder.Build())
	, _renderPassInfo(std::move(renderPassBuilder.BuildInfo()))
{
}

AirEngine::Runtime::Graphic::Instance::RenderPassBase::~RenderPassBase()
{
	if (_vkRenderPass)
	{
		vk::Device device(Core::Manager::GraphicDeviceManager::VkDevice());
		device.destroyRenderPass(_vkRenderPass);
	}
}

AirEngine::Runtime::Graphic::Instance::DummyRenderPass::DummyRenderPass()
	: RenderPassBase(
		RenderPassBase::RenderPassBuilder()
		.SetName("SampleRenderPass")
		.AddColorAttachment(
			"ColorAttachment",
			vk::Format::eR8G8B8A8Srgb,
			vk::AttachmentLoadOp::eClear, vk::AttachmentStoreOp::eStore,
			vk::ImageLayout::eColorAttachmentOptimal, vk::ImageLayout::eColorAttachmentOptimal
		)
		.AddDependency(
			"ExternalSubpass", "DrawSubpass",
			vk::PipelineStageFlagBits2::eNone, vk::PipelineStageFlagBits2::eNone,
			vk::AccessFlagBits2::eNone, vk::AccessFlagBits2::eNone
		)
		.AddSubpass(
			RenderPassBase::RenderSubpassBuilder()
			.SetName("DrawSubpass")
			.SetPipelineBindPoint(vk::PipelineBindPoint::eGraphics)
			.AddColorAttachment("ColorAttachment", vk::ImageLayout::eColorAttachmentOptimal)
		)
		.AddDependency(
			"DrawSubpass", "ExternalSubpass",
			vk::PipelineStageFlagBits2::eNone, vk::PipelineStageFlagBits2::eNone,
			vk::AccessFlagBits2::eNone, vk::AccessFlagBits2::eNone
		)
	)
{
}

#include "moc_RenderPassBase.cpp"
#include "RenderPassBase.moc"