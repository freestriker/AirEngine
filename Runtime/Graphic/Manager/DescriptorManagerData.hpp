#pragma once
#include "../../Utility/ContructorMacro.hpp"
#include "../../Utility/ExportMacro.hpp"
#include <map>
#include <vulkan/vulkan.hpp>

namespace AirEngine
{
	namespace Runtime
	{
		namespace Graphic
		{
			namespace Manager
			{
				class DescriptorManager;
				struct DescriptorMemoryHandle
				{
					friend class DescriptorManager;
				private:
					uint32_t offset;
					uint32_t size;
					DescriptorMemoryHandle(uint32_t offset, uint32_t size)
						: offset(offset)
						, size(size)
					{

					}
					DescriptorMemoryHandle()
						: offset(0)
						, size(0)
					{

					}
				public:
					size_t Offset()const;
					size_t Size()const;
				};

			}
		}
	}
}