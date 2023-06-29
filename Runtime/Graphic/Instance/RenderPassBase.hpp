#pragma once
#include <rttr/type>
#include <rttr/registration>
#include <map>
#include <string>
#include <vulkan/vulkan.hpp>
#include "../../Utility/ExportMacro.hpp"
#include "../../Utility/ContructorMacro.hpp"
#include "../../Utility/ReflectableObject.hpp"

namespace AirEngine
{
	namespace Runtime
	{
		namespace Graphic
		{
			namespace Instance
			{
				class AIR_ENGINE_API RenderPassBase
					: public Utility::ReflectableObject
				{
					REFLECTABLE_OBJECT
				public:
					class RenderPassBuilder;
					class RenderSubpassBuilder
					{
						friend class RenderPassBuilder;
					private:
						std::string _name;				
						vk::PipelineBindPoint _pipelineBindPoint;
						std::map<std::string, vk::AttachmentReference2> _coloreReferences;
						std::optional<std::pair<std::string, vk::AttachmentReference2>> _depthReference;
						std::map<std::string, vk::AttachmentReference2> _inputReferences;
						std::vector<std::string> _preserveReferences;
					public:
						RenderSubpassBuilder& SetName(const std::string& name);
						RenderSubpassBuilder& SetPipelineBindPoint(vk::PipelineBindPoint pipelineBindPoint);
						RenderSubpassBuilder& AddColorAttachment(const std::string& name, vk::ImageLayout layout);
						RenderSubpassBuilder& AddDepthAttachment(const std::string& name, vk::ImageLayout layout);
						RenderSubpassBuilder& AddPreserveAttachment(const std::string& namet);
						RenderSubpassBuilder& AddInputAttachment(const std::string& name, vk::ImageLayout layout, vk::ImageAspectFlags aspectMask);
					};
					class RenderPassBuilder
					{
						friend class RenderPassBase;
					public:
						struct AttachmentDescriptor
						{
							std::string name;
							vk::Format format;
							vk::AttachmentLoadOp loadOp;
							vk::AttachmentStoreOp storeOp;
							vk::AttachmentLoadOp stencilLoadOp;
							vk::AttachmentStoreOp stencilStoreOp;
							vk::ImageLayout initialLayout;
							vk::ImageLayout finalLayout;
						};
						struct DependencyDescriptor
						{
							std::string srcSubpass;
							std::string dstSubpass;
							vk::PipelineStageFlags2 srcStageMask;
							vk::PipelineStageFlags2 dstStageMask;
							vk::AccessFlags2 srcAccessMask;
							vk::AccessFlags2 dstAccessMask;
							vk::DependencyFlags dependencyFlags;
						};
					public:
						RenderPassBuilder& SetName(const std::string& name);
						RenderPassBuilder& AddColorAttachment(
							const std::string& name, 
							vk::Format format, 
							vk::AttachmentLoadOp loadOp, vk::AttachmentStoreOp storeOp, 
							vk::ImageLayout initialLayout, vk::ImageLayout finalLayout
						);
						RenderPassBuilder& AddSubpass(const RenderSubpassBuilder& renderSubpassBuilder);
						RenderPassBuilder& AddDependency(
							const std::string& srcSubpass,
							const std::string& dstSubpass,
							vk::PipelineStageFlags2 srcStageMask, vk::PipelineStageFlags2 dstStageMask,
							vk::AccessFlags2 srcAccessMask, vk::AccessFlags2 dstAccessMask,
							vk::DependencyFlags dependencyFlags = {}
						);
					private:
						vk::RenderPass Build() const;
						std::string _name;
						std::map<std::string, vk::AttachmentDescription2> _vkAttachmentDescriptionMap;
						std::vector<RenderSubpassBuilder> _renderSubpassBuilders;
						std::vector<std::tuple<std::string, std::string, vk::DependencyFlags, vk::MemoryBarrier2>> _srcDstDependencyBarriers;
					};
				private:
					vk::RenderPass _vkRenderPass;
				public:
					INVOKABLE RenderPassBase(const RenderPassBuilder& renderPassBuilder);
					INVOKABLE virtual ~RenderPassBase();
					NO_COPY_MOVE(RenderPassBase);
				};

				class AIR_ENGINE_API DummyRenderPass final
					: public RenderPassBase
				{
					REFLECTABLE_OBJECT
				public:
					INVOKABLE DummyRenderPass();
					INVOKABLE ~DummyRenderPass() = default;
					NO_COPY_MOVE(DummyRenderPass);
				};
			}
		}
	}
}