#pragma once
#include <vulkan/vulkan.hpp>
#include "AirEngine/Runtime/Utility/ContructorMacro.hpp"
#include "AirEngine/Runtime/Utility/ExportMacro.hpp"
#include "AirEngine/Runtime/Utility/InternedString.hpp"

namespace AirEngine
{
	namespace Runtime
	{
		namespace Graphic
		{
			namespace Manager
			{
				class DeviceManager;
			}
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
					friend class Manager::DeviceManager;
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
					std::mutex _mutex;
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
					inline std::mutex& Mutex()
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
						std::unique_lock< std::mutex> lock(_mutex);

						Submit({ submitInfo }, &fence);
					}
					void ImmediateConcentrateSubmit(const Command::Fence& fence = *static_cast<Command::Fence*>(nullptr))
					{
						std::unique_lock< std::mutex> lock(_mutex);

						Submit(_submitInfos, &fence);
						_submitInfos.clear();
					}
					void ImmediateConcentrateSubmit(const CommandBufferSubmitInfo& submitInfo, const Command::Fence& fence = *static_cast<Command::Fence*>(nullptr))
					{
						std::unique_lock< std::mutex> lock(_mutex);

						_submitInfos.emplace_back(submitInfo);
						Submit(_submitInfos, &fence);
						_submitInfos.clear();
					}
					inline void DelayedConcentrateSubmit(const CommandBufferSubmitInfo& submitInfo, const Command::Fence& fence = *static_cast<Command::Fence*>(nullptr))
					{
						std::unique_lock< std::mutex> lock(_mutex);

						_submitInfos.emplace_back(submitInfo);
					}
				};
			}
		}
	}
}