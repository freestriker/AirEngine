#include "RenderPassBase.hpp"
#include "../../Core/Manager/GraphicDeviceManager.hpp"

REGISTRATION
{
	DECLARE_TYPE(AirEngine::Runtime::Graphic::Instance::RenderPassBase*)
	DECLARE_TYPE(AirEngine::Runtime::Graphic::Instance::DummyRenderPass*)
}

AirEngine::Runtime::Graphic::Instance::RenderPassBase::RenderPassBuilder& AirEngine::Runtime::Graphic::Instance::RenderPassBase::RenderPassBuilder::SetName(const std::string& name)
{
	_name = Utility::InternedString(name);
	return *this;
}

AirEngine::Runtime::Graphic::Instance::RenderPassBase::RenderPassBuilder& AirEngine::Runtime::Graphic::Instance::RenderPassBase::RenderPassBuilder::AddColorAttachment(
	const std::string& name,
	vk::Format format,
	vk::AttachmentLoadOp loadOp, vk::AttachmentStoreOp storeOp,
	vk::ImageLayout initialLayout, vk::ImageLayout finalLayout
)
{
	auto&& key = Utility::InternedString(name);

	_attachmentNameToIndexMap.emplace(key, uint32_t(_vkAttachmentDescriptions.size()));
	_vkAttachmentDescriptions.emplace_back(
		vk::AttachmentDescriptionFlags(0),
		format,
		vk::SampleCountFlagBits::e1,
		loadOp, storeOp,
		vk::AttachmentLoadOp::eDontCare, vk::AttachmentStoreOp::eDontCare,
		initialLayout, finalLayout
	);
	return *this;
}

AirEngine::Runtime::Graphic::Instance::RenderPassBase::RenderPassBuilder& AirEngine::Runtime::Graphic::Instance::RenderPassBase::RenderPassBuilder::AddSubpass(const RenderSubpassBuilder& renderSubpassBuilder)
{
	_subpassNameToIndexMap.emplace(renderSubpassBuilder._name, uint32_t(_renderSubpassBuilders.size()));
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
		Utility::InternedString(srcSubpass), Utility::InternedString(dstSubpass),
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
	std::vector<vk::SubpassDescription2> vkSubpassDescriptions{_renderSubpassBuilders.size()};
	std::vector<vk::AttachmentReference2> vkAttachmentReferences{};
	uint32_t totalAttachmentReferenceCount = 0;
	std::vector<uint32_t> preserveAttachmentIndexs{};
	uint32_t totalPreserveAttachmentCount = 0;
	{
		_subpassNameToIndexMap[Utility::InternedString("ExternalSubpass")] = VK_SUBPASS_EXTERNAL;
		for (uint32_t subpassIndex = 0; subpassIndex < _renderSubpassBuilders.size(); subpassIndex++)
		{
			const auto& subpassBuilder = _renderSubpassBuilders[subpassIndex];
			auto& vkSubpassDescription = vkSubpassDescriptions[subpassIndex];

			_subpassNameToIndexMap[subpassBuilder._name] = subpassIndex;

			vkSubpassDescription.flags = vk::SubpassDescriptionFlags(0);
			vkSubpassDescription.pipelineBindPoint = vkSubpassDescription.pipelineBindPoint;
			vkSubpassDescription.viewMask = 0;
			vkSubpassDescription.pNext = nullptr;

			//input
			vkSubpassDescription.inputAttachmentCount = subpassBuilder._inputReferences.size();
			vkSubpassDescription.pInputAttachments = reinterpret_cast<vk::AttachmentReference2*>(size_t(totalAttachmentReferenceCount));
			totalAttachmentReferenceCount += subpassBuilder._inputReferences.size();

			//color
			vkSubpassDescription.colorAttachmentCount = subpassBuilder._coloreReferences.size();
			vkSubpassDescription.pColorAttachments = reinterpret_cast<vk::AttachmentReference2*>(size_t(totalAttachmentReferenceCount));
			totalAttachmentReferenceCount += subpassBuilder._coloreReferences.size();

			//resolve
			vkSubpassDescription.pResolveAttachments = nullptr;
			totalAttachmentReferenceCount += 0;

			//depth
			if (subpassBuilder._depthReference)
			{
				vkSubpassDescription.pDepthStencilAttachment = reinterpret_cast<vk::AttachmentReference2*>(size_t(totalAttachmentReferenceCount));
				totalAttachmentReferenceCount += 1;
			}
			else
			{
				vkSubpassDescription.pDepthStencilAttachment = nullptr;
				totalAttachmentReferenceCount += 0;
			}

			//preserve
			vkSubpassDescription.preserveAttachmentCount = subpassBuilder._preserveReferences.size();
			vkSubpassDescription.pPreserveAttachments = reinterpret_cast<uint32_t*>(size_t(totalPreserveAttachmentCount));
			totalPreserveAttachmentCount += subpassBuilder._preserveReferences.size();
		}

		vkAttachmentReferences.reserve(totalAttachmentReferenceCount);
		preserveAttachmentIndexs.reserve(totalPreserveAttachmentCount);

		for (uint32_t subpassIndex = 0; subpassIndex < _renderSubpassBuilders.size(); subpassIndex++)
		{
			const auto& subpassBuilder = _renderSubpassBuilders[subpassIndex];
			auto& vkSubpassDescription = vkSubpassDescriptions[subpassIndex];
			
			//input
			for (const auto& inputReferencePair: subpassBuilder._inputReferences)
			{
				vkAttachmentReferences.emplace_back(inputReferencePair.second);
				vkAttachmentReferences.back().attachment = _attachmentNameToIndexMap[inputReferencePair.first];
			}
			vkSubpassDescription.pInputAttachments = vkAttachmentReferences.data() + reinterpret_cast<size_t>(vkSubpassDescription.pInputAttachments);

			//color
			for (const auto& colorReferencePair: subpassBuilder._coloreReferences)
			{
				vkAttachmentReferences.emplace_back(colorReferencePair.second);
				vkAttachmentReferences.back().attachment = _attachmentNameToIndexMap[colorReferencePair.first];
			}
			vkSubpassDescription.pColorAttachments = vkAttachmentReferences.data() + reinterpret_cast<size_t>(vkSubpassDescription.pColorAttachments);

			//depth
			if (subpassBuilder._depthReference)
			{
				vkAttachmentReferences.emplace_back(subpassBuilder._depthReference.value().second);
				vkAttachmentReferences.back().attachment = _attachmentNameToIndexMap[subpassBuilder._depthReference.value().first];
				vkSubpassDescription.pDepthStencilAttachment = vkAttachmentReferences.data() + reinterpret_cast<size_t>(vkSubpassDescription.pDepthStencilAttachment);
			}
			else
			{
				vkSubpassDescription.pDepthStencilAttachment = nullptr;
			}

			//preserve
			for (const auto& preserveReference : subpassBuilder._preserveReferences)
			{
				preserveAttachmentIndexs.emplace_back(_attachmentNameToIndexMap[preserveReference]);
			}
			vkSubpassDescription.pPreserveAttachments = preserveAttachmentIndexs.data() + reinterpret_cast<size_t>(vkSubpassDescription.pPreserveAttachments);
		}
	}

	std::vector<vk::SubpassDependency2> vkSubpassDependencys{_srcDstDependencyBarriers.size(), vk::SubpassDependency2{}};
	{
		for (uint32_t dependencyIndex = 0; dependencyIndex < _srcDstDependencyBarriers.size(); dependencyIndex++)
		{
			const auto& tuple = _srcDstDependencyBarriers[dependencyIndex];
			auto& vkSubpassDependency = vkSubpassDependencys[dependencyIndex];
			auto& vkMemoryBarrier = std::get<3>(tuple);

			vkSubpassDependency.srcSubpass = _subpassNameToIndexMap[std::get<0>(tuple)];
			vkSubpassDependency.dstSubpass = _subpassNameToIndexMap[std::get<1>(tuple)];
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
	vkRenderPassCreateInfo.attachmentCount = _vkAttachmentDescriptions.size();
	vkRenderPassCreateInfo.pAttachments = _vkAttachmentDescriptions.data();
	vkRenderPassCreateInfo.subpassCount = vkSubpassDescriptions.size();
	vkRenderPassCreateInfo.pSubpasses = vkSubpassDescriptions.data();
	vkRenderPassCreateInfo.dependencyCount = vkSubpassDependencys.size();
	vkRenderPassCreateInfo.pDependencies = vkSubpassDependencys.data();
	vkRenderPassCreateInfo.correlatedViewMaskCount = 0;
	vkRenderPassCreateInfo.pCorrelatedViewMasks = nullptr;
	vkRenderPassCreateInfo.pNext = nullptr;

	return Core::Manager::GraphicDeviceManager::Device().createRenderPass2(vkRenderPassCreateInfo);
}

AirEngine::Runtime::Graphic::Instance::RenderPassBase::RenderPassInfo AirEngine::Runtime::Graphic::Instance::RenderPassBase::RenderPassBuilder::BuildInfo() const
{
	RenderPassInfo renderPassInfo{};
	renderPassInfo._name = Utility::InternedString(_name);

	std::unordered_map<Utility::InternedString, SubPassInfo>& subPassInfos = renderPassInfo._subPassInfos;
	for (uint32_t subpassIndex = 0; subpassIndex < _renderSubpassBuilders.size(); subpassIndex++)
	{
		const auto& subpassBuilder = _renderSubpassBuilders[subpassIndex];

		SubPassInfo subPassInfo{};
		subPassInfo._name = Utility::InternedString(subpassBuilder._name);
		subPassInfo._index = subpassIndex;

		for (uint32_t coloreReferenceIndex = 0; coloreReferenceIndex < subpassBuilder._coloreReferences.size(); ++coloreReferenceIndex)
		{
			auto&& pair = subpassBuilder._coloreReferences[coloreReferenceIndex];

			AttachmentInfo attachmentInfo{};
			attachmentInfo.name = Utility::InternedString(pair.first);
			attachmentInfo.attachmentInfoIndex = uint32_t(subPassInfo._attachments.size());
			attachmentInfo.attachmentIndex = _attachmentNameToIndexMap.at(pair.first);
			attachmentInfo.location = coloreReferenceIndex;
			attachmentInfo.format = _vkAttachmentDescriptions.at(_attachmentNameToIndexMap.at(pair.first)).format;
			attachmentInfo.layout = pair.second.layout;
			attachmentInfo.type = AttachmentType::COLOR;

			subPassInfo._nameToIndexMap.emplace(attachmentInfo.name, attachmentInfo.attachmentInfoIndex);

			subPassInfo._attachments.emplace_back(attachmentInfo);
		}
		if (subpassBuilder._depthReference)
		{
			const auto& pair = subpassBuilder._depthReference.value();

			AttachmentInfo attachmentInfo{};
			attachmentInfo.name = Utility::InternedString(pair.first);
			attachmentInfo.attachmentInfoIndex = uint32_t(subPassInfo._attachments.size());
			attachmentInfo.attachmentIndex = _attachmentNameToIndexMap.at(pair.first);
			attachmentInfo.location = -1;
			attachmentInfo.format = _vkAttachmentDescriptions.at(_attachmentNameToIndexMap.at(pair.first)).format;
			attachmentInfo.layout = pair.second.layout;
			attachmentInfo.type = AttachmentType::DEPTH;

			subPassInfo._nameToIndexMap.emplace(attachmentInfo.name, attachmentInfo.attachmentInfoIndex);

			subPassInfo._attachments.emplace_back(attachmentInfo);
		}
		for (uint32_t inputReferenceIndex = 0; inputReferenceIndex < subpassBuilder._inputReferences.size(); ++inputReferenceIndex)
		{
			auto&& pair = subpassBuilder._inputReferences[inputReferenceIndex];

			AttachmentInfo attachmentInfo{};
			attachmentInfo.name = Utility::InternedString(pair.first);
			attachmentInfo.attachmentInfoIndex = uint32_t(subPassInfo._attachments.size());
			attachmentInfo.attachmentIndex = _attachmentNameToIndexMap.at(pair.first);
			attachmentInfo.location = inputReferenceIndex;
			attachmentInfo.format = _vkAttachmentDescriptions.at(_attachmentNameToIndexMap.at(pair.first)).format;
			attachmentInfo.layout = pair.second.layout;
			attachmentInfo.type = AttachmentType::INPUT;

			subPassInfo._nameToIndexMap.emplace(attachmentInfo.name, attachmentInfo.attachmentInfoIndex);

			subPassInfo._attachments.emplace_back(attachmentInfo);
		}

		subPassInfos.emplace(subPassInfo._name, subPassInfo);
	}

	return renderPassInfo;
}

AirEngine::Runtime::Graphic::Instance::RenderPassBase::RenderSubpassBuilder& AirEngine::Runtime::Graphic::Instance::RenderPassBase::RenderSubpassBuilder::SetName(const std::string& name)
{
	_name = Utility::InternedString(name);
	return *this;
}

AirEngine::Runtime::Graphic::Instance::RenderPassBase::RenderSubpassBuilder& AirEngine::Runtime::Graphic::Instance::RenderPassBase::RenderSubpassBuilder::SetPipelineBindPoint(vk::PipelineBindPoint pipelineBindPoint)
{
	_pipelineBindPoint = pipelineBindPoint;
	return *this;
}

AirEngine::Runtime::Graphic::Instance::RenderPassBase::RenderSubpassBuilder& AirEngine::Runtime::Graphic::Instance::RenderPassBase::RenderSubpassBuilder::AddColorAttachment(const std::string& name, vk::ImageLayout layout)
{
	_coloreReferences.emplace_back(
		Utility::InternedString(name),
		vk::AttachmentReference2{
			uint32_t(-1),
			layout,
			vk::ImageAspectFlags(0)
		}
	);
	return *this;
}

AirEngine::Runtime::Graphic::Instance::RenderPassBase::RenderSubpassBuilder& AirEngine::Runtime::Graphic::Instance::RenderPassBase::RenderSubpassBuilder::AddDepthAttachment(const std::string& name, vk::ImageLayout layout)
{
	_depthReference =
	{
		Utility::InternedString(name),
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
	_preserveReferences.emplace_back(Utility::InternedString(name));
	return *this;
}

AirEngine::Runtime::Graphic::Instance::RenderPassBase::RenderSubpassBuilder& AirEngine::Runtime::Graphic::Instance::RenderPassBase::RenderSubpassBuilder::AddInputAttachment(const std::string& name, vk::ImageLayout layout, vk::ImageAspectFlags aspectMask)
{
	_inputReferences.emplace_back(
		Utility::InternedString(name),
		vk::AttachmentReference2{
			uint32_t(-1),
			layout,
			aspectMask
		}
	);
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
		Core::Manager::GraphicDeviceManager::Device().destroyRenderPass(_vkRenderPass);
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
//#include "RenderPassBase.moc"