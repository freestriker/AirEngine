#pragma once
#include "AirEngine/Runtime/Utility/ExportMacro.hpp"
#include "AirEngine/Runtime/Utility/ContructorMacro.hpp"
#include <vector>

namespace AirEngine
{
	namespace Runtime
	{
		namespace Graphic
		{
			namespace Rendering
			{
				class AIR_ENGINE_API MaterialBindableAssetBase
				{
				private:
					std::vector<uint8_t> _rawDescriptor;
				protected:
				public:
					inline std::vector<uint8_t>& RawDescriptor()
					{
						return _rawDescriptor;
					}
					MaterialBindableAssetBase() = default;
					virtual ~MaterialBindableAssetBase() = default;
					NO_COPY_MOVE(MaterialBindableAssetBase);

				};
			}
		}
	}
}