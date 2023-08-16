#pragma once
#include "../../Utility/InternedString.hpp"
#include "../Manager/DescriptorManagerData.hpp"
#include "MaterialBindableAssetBase.hpp"

namespace AirEngine
{
	namespace Runtime
	{
		namespace Graphic
		{
			namespace Rendering
			{
				struct MaterialBindableAssetInfo
				{
					Utility::InternedString name;
					MaterialBindableAssetBase* bindable;
					uint16_t index;
				};
				struct MaterialDescriptorSetMemoryInfo
				{
					Manager::DescriptorMemoryHandle handle;
					uint16_t dynamicCount;
					uint16_t dynamicCapcity;
				};
			}
		}
	}
}