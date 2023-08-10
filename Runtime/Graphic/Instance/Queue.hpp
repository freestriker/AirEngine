#pragma once
#include <vulkan/vulkan.hpp>
#include "../../Utility/ContructorMacro.hpp"
#include "../../Utility/ExportMacro.hpp"
#include "../../Utility/Fiber.hpp"
#include "../../Utility/InternedString.hpp"

namespace AirEngine
{
	namespace Runtime
	{
		namespace Core
		{
			namespace Manager
			{
				class GraphicDeviceManager;
			}
		}
		namespace Graphic
		{
			namespace Command
			{
				class Semaphore;
				class CommandBuffer;
				class Fence;
			}
			namespace Instance
			{
				class AIR_ENGINE_API Queue final
				{
					friend class Core::Manager::GraphicDeviceManager;
				public:
					struct CommandBufferSubmitSemaphoreInfo
					{
						Command::Semaphore* semphore;
						vk::PipelineStageFlags2 stageMask;
					};
					struct CommandBufferSubmitInfo
					{
						std::vector<CommandBufferSubmitSemaphoreInfo> waitInfos;
						std::vector<Command::CommandBuffer*> commandBuffers;
						std::vector<CommandBufferSubmitSemaphoreInfo> signalInfos;
					};
				private:
					vk::Queue _vkQueue;
					Utility::Fiber::mutex _mutex;
					Utility::InternedString _name;
					uint32_t _familyIndex;
					std::vector<CommandBufferSubmitInfo> _submitInfos;
					Queue(vk::Queue queue, uint32_t familyIndex, Utility::InternedString name)
						: _vkQueue(queue)
						, _mutex()
						, _name(name)
						, _familyIndex(familyIndex)
						, _submitInfos()
					{

					}
					NO_COPY_MOVE(Queue)
					void Submit(const std::vector<CommandBufferSubmitInfo>& submitInfos, const Command::Fence* fence);
				public:
					~Queue() = default;
					inline Utility::Fiber::mutex& Mutex()
					{
						return _mutex;
					}
					inline vk::Queue VkHandle() const
					{
						return _vkQueue;
					}
					inline Utility::InternedString Name()const
					{
						return _name;
					}
					inline uint32_t FamilyIndex()const
					{
						return _familyIndex;
					}

					void ImmediateIndividualSubmit(const CommandBufferSubmitInfo& submitInfo, const Command::Fence& fence = *static_cast<Command::Fence*>(nullptr))
					{
						std::unique_lock< Utility::Fiber::mutex> lock(_mutex);

						Submit({ submitInfo }, &fence);
					}
					void ImmediateConcentrateSubmit(const Command::Fence& fence = *static_cast<Command::Fence*>(nullptr))
					{
						std::unique_lock< Utility::Fiber::mutex> lock(_mutex);

						Submit(_submitInfos, &fence);
						_submitInfos.clear();
					}
					void ImmediateConcentrateSubmit(const CommandBufferSubmitInfo& submitInfo, const Command::Fence& fence = *static_cast<Command::Fence*>(nullptr))
					{
						std::unique_lock< Utility::Fiber::mutex> lock(_mutex);

						_submitInfos.emplace_back(submitInfo);
						Submit(_submitInfos, &fence);
						_submitInfos.clear();
					}
					inline void DelayedConcentrateSubmit(const CommandBufferSubmitInfo& submitInfo, const Command::Fence& fence = *static_cast<Command::Fence*>(nullptr))
					{
						std::unique_lock< Utility::Fiber::mutex> lock(_mutex);

						_submitInfos.emplace_back(submitInfo);
					}
				};
			}
		}
	}
}