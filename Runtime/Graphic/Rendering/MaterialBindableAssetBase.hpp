#pragma once
#include "AirEngine/Runtime/Utility/ExportMacro.hpp"
#include "AirEngine/Runtime/Utility/ContructorMacro.hpp"
#include <vector>
#include <vulkan/vulkan.hpp>

namespace AirEngine
{
	namespace Runtime
	{
		namespace Graphic
		{
			namespace Rendering
			{
				class Material;
				class AIR_ENGINE_API MaterialBindableAssetBase
				{
					friend class Material;
				protected:
					virtual void SetDescriptorData(uint8_t* targetPtr, vk::DescriptorType descriptorType)
					{

					}
				public:
					MaterialBindableAssetBase() = default;
					virtual ~MaterialBindableAssetBase() = default;
					NO_COPY_MOVE(MaterialBindableAssetBase);
				};
			}
		}
	}
}