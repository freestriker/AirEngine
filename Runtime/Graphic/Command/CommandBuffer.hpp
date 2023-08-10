#pragma once
#include <vulkan/vulkan.hpp>
#include "../../Utility/ContructorMacro.hpp"
#include "../../Utility/ExportMacro.hpp"
#include "../../Utility/InternedString.hpp"
#include <glm/vec4.hpp>

namespace AirEngine
{
	namespace Runtime
	{
		namespace Graphic
		{
			namespace Instance
			{
				class Queue;
				class Image;
				class Buffer;
			}
			namespace Command
			{
				class CommandPool;
				class Barrier;
				class AIR_ENGINE_API CommandBuffer final
				{
					friend class CommandPool;
				private:
					Utility::InternedString _name;
					vk::CommandBuffer _vkCommandBuffer;
					vk::CommandBufferLevel _vkCommandBufferLevel;
					CommandPool* _commandPool;

					CommandBuffer(Utility::InternedString commandBufferName, Command::CommandPool* commandPool, vk::CommandBufferLevel level = vk::CommandBufferLevel::ePrimary);
					NO_COPY_MOVE(CommandBuffer)
				public:
					~CommandBuffer();

					inline vk::CommandBuffer VkHandle()
					{
						return _vkCommandBuffer;
					}
					void Reset();
					void BeginRecord(vk::CommandBufferUsageFlags flags = {});
					void EndRecord();
					void AddPipelineBarrier(const Barrier& barrier, vk::DependencyFlags dependencyFlags = {});
					void ClearColorImage(const Instance::Image& image, vk::ImageLayout imageLayout, const vk::ClearColorValue& color);
					template<typename TColorChannel>
					void ClearColorImage(const Instance::Image& image, vk::ImageLayout imageLayout, const glm::vec<4, TColorChannel, glm::defaultp>& color);
					void CopyBufferToImage(const Instance::Buffer& buffer, const Instance::Image& image, vk::ImageLayout imageLayout, vk::ImageAspectFlags imageAspectFlags);
					void CopyBuffer(const Instance::Buffer& srcBuffer, const Instance::Buffer& dstBuffer, const std::vector<std::tuple<vk::DeviceSize, vk::DeviceSize, vk::DeviceSize>> srcOffsetDstOffsetSizes);
					void Blit(const Instance::Image& srcImage, vk::ImageLayout srcImageLayout, const Instance::Image& dstImage, vk::ImageLayout dstImageLayout, vk::ImageAspectFlags imageAspectFlags, vk::Filter filter);
				};
				template<typename TColorChannel>
				inline void CommandBuffer::ClearColorImage(const Instance::Image& image, vk::ImageLayout imageLayout, const glm::vec<4, TColorChannel, glm::defaultp>& color)
				{
					vk::ClearColorValue vkColor{};
					auto&& vkColorRef = reinterpret_cast<glm::vec<4, TColorChannel, glm::defaultp>&>(vkColor);
					vkColorRef = color;

					ClearColorImage(image, imageLayout, vkColor);
				}
			}
		}
	}
}