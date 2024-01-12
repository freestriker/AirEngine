#pragma once
#include <map>
#include "AirEngine/Runtime/Utility/InternedString.hpp"
#include <vulkan/vulkan.hpp>
#include "AirEngine/Runtime/Utility/ExportMacro.hpp"
#include "AirEngine/Runtime/Utility/ContructorMacro.hpp"

namespace AirEngine
{
	namespace Runtime
	{
		namespace Graphic
		{
			namespace Instance
			{
				class ImageView;
				class RenderPassBase;
				class FrameBufferBuilder;
				class AIR_ENGINE_API FrameBuffer final
				{
					friend class FrameBufferBuilder;
				private:
					vk::Framebuffer _vkFrameBuffer;
					std::map<Utility::InternedString, Instance::ImageView*> _attachments;
					vk::Extent2D _extent2D;
					RenderPassBase* _renderPass;
				private:
					FrameBuffer();
				public:
					FrameBuffer(const FrameBufferBuilder& builder);
					~FrameBuffer();
					NO_COPY_MOVE(FrameBuffer);

					inline vk::Framebuffer VkHandle() const
					{ 
						return _vkFrameBuffer;
					}
					inline vk::Extent2D Extent2D() const
					{ 
						return _extent2D;
					}
					inline RenderPassBase* RenderPassBase() const
					{ 
						return _renderPass;
					}
					inline std::map<Utility::InternedString, Instance::ImageView*> Attachments() const
					{
						return _attachments;
					}
				};

				class AIR_ENGINE_API FrameBufferBuilder final
				{
					friend class FrameBuffer;
				private:
					std::map<Utility::InternedString, Instance::ImageView*> _attachmentMap;
					std::vector<vk::ImageView> _vkImageViews;
					vk::Extent2D _extent2D;
					RenderPassBase* _renderPass;
				public:
					FrameBufferBuilder();
					FrameBufferBuilder& SetRenderPass(RenderPassBase* renderPass);
					FrameBufferBuilder& SetAttachment(Utility::InternedString, Instance::ImageView*);
					FrameBuffer* Build();
				};
			}
		}
	}
}