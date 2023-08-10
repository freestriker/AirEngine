#pragma once
#include "ContructorMacro.hpp"
#include "ExportMacro.hpp"
#include <unordered_map>
#include <string>
#include <vulkan/vulkan.hpp>
#include <opencv2/core/hal/interface.h>
#include <vulkan/vulkan_format_traits.hpp>

namespace AirEngine
{
	namespace Runtime
	{
		namespace Utility
		{
			class AIR_ENGINE_API VulkanOpenCVTypeTransfer final
			{
			private:
				VulkanOpenCVTypeTransfer();
				~VulkanOpenCVTypeTransfer() = default;
				NO_COPY_MOVE(VulkanOpenCVTypeTransfer);

			public:
				using PerChannelValueTypeIndex = int8_t;
				static constexpr int8_t UNVALID_PER_CHANNEL_VALUE_TYPE_INDEX = -1;
				static constexpr int8_t SIGNED_PER_CHANNEL_VALUE_TYPE_INDEX = 0;
				static constexpr int8_t UNSIGNED_PER_CHANNEL_VALUE_TYPE_INDEX = 1;
				static constexpr int8_t FLOAT_PER_CHANNEL_VALUE_TYPE_INDEX = 2;

				using PerChannelBitsIndex = int8_t;
				static constexpr int8_t UNVALID_PER_CHANNEL_BITS_INDEX = -1;
				static constexpr int8_t BITS8_PER_CHANNEL_BITS_INDEX = 0;
				static constexpr int8_t BITS16_PER_CHANNEL_BITS_INDEX = 1;
				static constexpr int8_t BITS32_PER_CHANNEL_BITS_INDEX = 2;
			private:
				static const std::unordered_map<std::string, PerChannelValueTypeIndex> _numericFormatStringToPerChannelValueTypeIndexMap;
				static const int _perChannelBitsIndexAndPerChannelValueTypeIndexToPerChannelValueTypeMap[3][3];
				static const std::unordered_map<std::string, VkFormat> _vkFormatStringToVkFormatMap;
				static const std::unordered_map<std::string, VkImageUsageFlagBits> _vkImageUsageFlagBitsStringToVkImageUsageFlagBitsMap;
				static const std::unordered_map<std::string, VkMemoryPropertyFlagBits> _vkMemoryPropertyFlagBitsStringToVkMemoryPropertyFlagBitsMap;
				static const std::unordered_map<std::string, VkImageLayout> _vkImageLayoutStringToVkImageLayoutMap;
				static const std::unordered_map<std::string, VkImageAspectFlagBits> _vkImageAspectFlagBitsStringToVkImageAspectFlagBitsMap;

			public:
				inline static PerChannelValueTypeIndex NumericFormatStringToPerChannelValueTypeIndex(const std::string& numericFormatString)
				{
					return _numericFormatStringToPerChannelValueTypeIndexMap.at(numericFormatString);
				}
				inline static PerChannelBitsIndex PerChannelBitsToPerChannelBitsIndex(uint8_t perChannelBits)
				{
					switch (perChannelBits)
					{
						case 8: return BITS8_PER_CHANNEL_BITS_INDEX;
						case 16: return BITS16_PER_CHANNEL_BITS_INDEX;
						case 32: return BITS32_PER_CHANNEL_BITS_INDEX;
						default: return UNVALID_PER_CHANNEL_BITS_INDEX;
					}
				}
				inline static int8_t PerChannelBitsIndexAndPerChannelValueTypeIndexToPerChannelValueType(PerChannelBitsIndex perChannelBitsIndex, PerChannelValueTypeIndex perChannelValueTypeIndex)
				{
					return _perChannelBitsIndexAndPerChannelValueTypeIndexToPerChannelValueTypeMap[perChannelBitsIndex][perChannelValueTypeIndex];
				}
				inline static vk::Format ParseToVkFormat(const std::string& formatString)
				{
					return vk::Format(_vkFormatStringToVkFormatMap.at(formatString));
				}
				inline static vk::ImageUsageFlagBits ParseToVkImageUsageFlagBits(const std::string& usageString)
				{
					return vk::ImageUsageFlagBits(_vkImageUsageFlagBitsStringToVkImageUsageFlagBitsMap.at(usageString));
				}
				inline static vk::MemoryPropertyFlagBits ParseToVkMemoryPropertyFlagBits(const std::string& propertyString)
				{
					return vk::MemoryPropertyFlagBits(_vkMemoryPropertyFlagBitsStringToVkMemoryPropertyFlagBitsMap.at(propertyString));
				}
				inline static vk::ImageLayout ParseToVkImageLayout(const std::string& propertyString)
				{
					return vk::ImageLayout(_vkImageLayoutStringToVkImageLayoutMap.at(propertyString));
				}
				inline static vk::ImageAspectFlagBits ParseToVkImageAspectFlagBits(const std::string& propertyString)
				{
					return vk::ImageAspectFlagBits(_vkImageAspectFlagBitsStringToVkImageAspectFlagBitsMap.at(propertyString));
				}
				inline static int VkFormatToPerChannelValueType(vk::Format format, std::string& error)
				{
					vk::Format originalFormat = vk::Format(format);

					Utility::VulkanOpenCVTypeTransfer::PerChannelBitsIndex originalCvImagePerChannelBitsIndex = Utility::VulkanOpenCVTypeTransfer::PerChannelBitsToPerChannelBitsIndex(vk::componentBits(originalFormat, 0));
					if (originalCvImagePerChannelBitsIndex == Utility::VulkanOpenCVTypeTransfer::UNVALID_PER_CHANNEL_BITS_INDEX)
					{
						error = "Can not parse this component bits.";
						return -1;
					}

					Utility::VulkanOpenCVTypeTransfer::PerChannelValueTypeIndex originalCvImagePerChannelValueTypeIndex = Utility::VulkanOpenCVTypeTransfer::NumericFormatStringToPerChannelValueTypeIndex(vk::componentNumericFormat(originalFormat, 0));
					if (originalCvImagePerChannelValueTypeIndex == Utility::VulkanOpenCVTypeTransfer::UNVALID_PER_CHANNEL_VALUE_TYPE_INDEX) 
					{
						error = "Can not parse this component numeric format.";
						return -1;
					}

					auto originalCvImagePerChannelValueType = Utility::VulkanOpenCVTypeTransfer::PerChannelBitsIndexAndPerChannelValueTypeIndexToPerChannelValueType(originalCvImagePerChannelBitsIndex, originalCvImagePerChannelValueTypeIndex);
					if (originalCvImagePerChannelValueType == -1)
					{
						error = "Can not parse this per channel value type.";
						return -1;
					}

					return originalCvImagePerChannelValueType;
				}
				inline static int VkFormatToValueType(vk::Format format, std::string& error)
				{
					vk::Format originalFormat = vk::Format(format);

					auto originalCvImagePerChannelValueType = VkFormatToPerChannelValueType(format, error);
					if (originalCvImagePerChannelValueType == -1) return -1;

					uint8_t originalCvImageChannelCount = vk::componentCount(originalFormat);
					return CV_MAKETYPE(originalCvImagePerChannelValueType, originalCvImageChannelCount);
				}
			};
		}
	}
}