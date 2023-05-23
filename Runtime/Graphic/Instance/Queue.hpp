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
			namespace Instance
			{
				class AIR_ENGINE_API Queue final
				{
					friend class Core::Manager::GraphicDeviceManager;
				private:
					VkQueue _queue;
					Utility::Fiber::mutex _mutex;
					Utility::InternedString _name;
					uint32_t _familyIndex;
					Queue(VkQueue queue, uint32_t familyIndex, Utility::InternedString name)
						: _queue(queue)
						, _mutex()
						, _name(name)
						, _familyIndex(familyIndex)
					{

					}
					NO_COPY_MOVE(Queue)
				public:
					~Queue() = default;
					inline Utility::Fiber::mutex& Mutex()
					{
						return _mutex;
					}
					inline VkQueue VkHandle() const
					{
						return _queue;
					}
					inline Utility::InternedString Name()const
					{
						return _name;
					}
					inline uint32_t FamilyIndex()const
					{
						return _familyIndex;
					}
				};
			}
		}
	}
}