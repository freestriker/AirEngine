#pragma once
#include <vulkan/vulkan_core.h>
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
						VkPipelineStageFlags2 stageMask;
					};
					struct CommandBufferSubmitInfo
					{
						std::vector<CommandBufferSubmitSemaphoreInfo> waitInfos;
						std::vector<Command::CommandBuffer*> commandBuffers;
						std::vector<CommandBufferSubmitSemaphoreInfo> signalInfos;
					};
				private:
					VkQueue _vkQueue;
					Utility::Fiber::mutex _mutex;
					Utility::InternedString _name;
					uint32_t _familyIndex;
					std::vector<CommandBufferSubmitInfo> _submitInfos;
					Queue(VkQueue queue, uint32_t familyIndex, Utility::InternedString name)
						: _vkQueue(queue)
						, _mutex()
						, _name(name)
						, _familyIndex(familyIndex)
						, _submitInfos()
					{

					}
					NO_COPY_MOVE(Queue)
					void Submit(const std::vector<CommandBufferSubmitInfo>& submitInfos);
				public:
					~Queue() = default;
					inline Utility::Fiber::mutex& Mutex()
					{
						return _mutex;
					}
					inline VkQueue VkHandle() const
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

					void ImmediateIndividualSubmit(const CommandBufferSubmitInfo& submitInfo)
					{
						std::unique_lock< Utility::Fiber::mutex> lock(_mutex);

						Submit({ submitInfo });
					}
					void ImmediateConcentrateSubmit()
					{
						std::unique_lock< Utility::Fiber::mutex> lock(_mutex);

						Submit(_submitInfos);
						_submitInfos.clear();
					}
					void ImmediateConcentrateSubmit(const CommandBufferSubmitInfo& submitInfo)
					{
						std::unique_lock< Utility::Fiber::mutex> lock(_mutex);

						_submitInfos.emplace_back(submitInfo);
						Submit(_submitInfos);
						_submitInfos.clear();
					}
					inline void DelayedConcentrateSubmit(const CommandBufferSubmitInfo& submitInfo)
					{
						std::unique_lock< Utility::Fiber::mutex> lock(_mutex);

						_submitInfos.emplace_back(submitInfo);
					}
				};
			}
		}
	}
}