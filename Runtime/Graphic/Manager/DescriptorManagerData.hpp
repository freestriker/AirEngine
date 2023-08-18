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
					uint32_t compressedOffset;
					uint32_t compressedSize;
					DescriptorMemoryHandle(uint32_t offset, uint32_t size)
						: compressedOffset(offset)
						, compressedSize(size)
					{

					}
				public:
					DescriptorMemoryHandle()
						: compressedOffset(0)
						, compressedSize(0)
					{

					}
					size_t Offset()const;
					size_t Size()const;
					inline uint32_t CompressedOffset()const
					{
						return compressedOffset;
					}
					inline uint32_t CompressedSize()const
					{
						return compressedSize;
					}
					inline uint32_t CompressedEndOffset()const
					{
						return compressedOffset + compressedSize;
					}
				};
			}
		}
	}
}