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
				};
				struct MaterialDescriptorSetMemoryInfo
				{
					Manager::DescriptorMemoryHandle handle;
					uint32_t dynamicElementCapcity;
				};
			}
		}
	}
}