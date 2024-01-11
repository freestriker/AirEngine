#pragma once
#include <rttr/type>
#include <rttr/registration>
#include <map>
#include <string>
#include <vulkan/vulkan.hpp>
#include "AirEngine/Runtime/Utility/ExportMacro.hpp"
#include "AirEngine/Runtime/Utility/ContructorMacro.hpp"
#include "AirEngine/Runtime/Utility/ReflectableObject.hpp"
#include "AirEngine/Runtime/Utility/InternedString.hpp"

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
					enum class AttachmentType
					{
						COLOR,
						DEPTH,
						INPUT
					};
					struct AttachmentInfo
					{
						Utility::InternedString name;
						uint32_t attachmentInfoIndex;
						uint32_t attachmentIndex;
						uint32_t location;
						vk::Format format;
						vk::ImageLayout layout;
						AttachmentType type;
					};
					struct SubPassInfo
					{
						friend class RenderPassBuilder;
					private:
						Utility::InternedString _name;
						uint32_t _index;
						std::vector<AttachmentInfo> _attachments;
						std::unordered_map< Utility::InternedString, uint32_t> _nameToIndexMap;
					public:
						inline Utility::InternedString Name() const
						{
							return _name;
						}
						inline const RenderPassBase::AttachmentInfo& AttachmentInfo(Utility::InternedString name) const
						{
							return _attachments.at(_nameToIndexMap.at(name));
						}
						inline uint32_t Index()const
						{
							return _index;
						}
					};
					struct RenderPassInfo
					{
						friend class RenderPassBuilder;
					private:
						Utility::InternedString _name;
						std::unordered_map<Utility::InternedString, SubPassInfo> _subPassInfos;
						std::unordered_map<Utility::InternedString, AttachmentInfo> _attachmentInfos;
					public:
						inline Utility::InternedString Name() const
						{
							return _name;
						}
						inline const SubPassInfo& SubPassInfo(Utility::InternedString name) const
						{
							return _subPassInfos.at(name);
						}
						inline const std::unordered_map<Utility::InternedString, AttachmentInfo>& AttachmentInfos() const
						{
							return _attachmentInfos;
						}
					};

					class RenderSubpassBuilder
					{
						friend class RenderPassBuilder;
					private:
						Utility::InternedString _name;
						vk::PipelineBindPoint _pipelineBindPoint;
						std::vector<std::pair<Utility::InternedString, vk::AttachmentReference2>> _coloreReferences;
						std::optional<std::pair<Utility::InternedString, vk::AttachmentReference2>> _depthReference;
						std::vector<std::pair<Utility::InternedString, vk::AttachmentReference2>> _inputReferences;
						std::vector<Utility::InternedString> _preserveReferences;
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
							Utility::InternedString name;
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
							Utility::InternedString srcSubpass;
							Utility::InternedString dstSubpass;
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
						RenderPassInfo BuildInfo() const;
						Utility::InternedString _name;
						mutable std::map<Utility::InternedString, uint32_t> _attachmentNameToIndexMap;
						mutable std::vector<vk::AttachmentDescription2> _vkAttachmentDescriptions;
						mutable std::map<Utility::InternedString, uint32_t> _subpassNameToIndexMap;
						mutable std::vector<RenderSubpassBuilder> _renderSubpassBuilders;
						mutable std::vector<std::tuple<Utility::InternedString, Utility::InternedString, vk::DependencyFlags, vk::MemoryBarrier2>> _srcDstDependencyBarriers;
					};
				private:
					vk::RenderPass _vkRenderPass;
					RenderPassInfo _renderPassInfo;
				public:
					INVOKABLE RenderPassBase(const RenderPassBuilder& renderPassBuilder);
					INVOKABLE virtual ~RenderPassBase();
					NO_COPY_MOVE(RenderPassBase);
					inline const RenderPassInfo& Info()const
					{
						return _renderPassInfo;
					}
					inline vk::RenderPass VkHandle()const
					{
						return _vkRenderPass;
					}
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