#pragma once
#include "AirEngine/Runtime/Utility/ContructorMacro.hpp"
#include "AirEngine/Runtime/Utility/ExportMacro.hpp"
#include <unordered_map>
#include <vulkan/vulkan.hpp>

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
			namespace Manager
			{
				class AIR_ENGINE_API ShaderManager final
				{
					friend class Core::Manager::GraphicDeviceManager;
				private:
					ShaderManager() = delete;
					~ShaderManager() = delete;
					NO_COPY_MOVE(ShaderManager)
				private:
					static std::unordered_map<vk::DescriptorType, uint8_t> _descriptorTypeToSizeMap;
					static uint8_t _offsetAlignment;
				private:
					static void Initialize();
				public:
					inline static uint8_t OffsetAlignment()
					{
						return _offsetAlignment;
					}
					inline static uint8_t DescriptorSize(vk::DescriptorType descriptorType)
					{
						return _descriptorTypeToSizeMap.at(descriptorType);
					}
				};
			}
		}
	}
}