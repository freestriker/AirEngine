#pragma once
#include "ContructorMacro.hpp"
#include "ExportMacro.hpp"
#include <unordered_map>
#include <string>
#include <vulkan/vulkan.hpp>
#include <vulkan/vulkan_format_traits.hpp>

namespace AirEngine
{
	namespace Runtime
	{
		namespace Utility
		{
			class AIR_ENGINE_API StringToVulkanypeTransfer final
			{
			private:
				StringToVulkanypeTransfer() = delete;
				~StringToVulkanypeTransfer() = delete;
				NO_COPY_MOVE(StringToVulkanypeTransfer);

			private:
				static const std::unordered_map<std::string, vk::BlendOp> _vkBlendOpStringToVkBlendOpMap;
				static const std::unordered_map<std::string, vk::BlendFactor> _vkBlendFactorStringToVkBlendFactorMap;
				static const std::unordered_map<std::string, vk::ColorComponentFlagBits> _vkColorComponentFlagBitsStringToVkColorComponentFlagBitsMap;
				static const std::unordered_map<std::string, vk::CullModeFlagBits> _vkCullModeFlagBitsStringToVkCullModeFlagBitsMap;
				static const std::unordered_map<std::string, vk::CompareOp> _vkCompareOpStringToVkCompareOpMap;
				static const std::unordered_map<std::string, vk::ImageLayout> _vkImageLayoutStringToVkImageLayoutMap;
				static const std::unordered_map<std::string, vk::Format> _vkFormatStringToVkFormatMap;
				static const std::unordered_map <std::string, vk::ImageUsageFlags> _vkImageUsageFlagBitsStringToVkImageUsageFlagsMap;
				static const std::unordered_map<std::string, vk::MemoryPropertyFlags> _vkMemoryPropertyFlagBitsStringToVkMemoryPropertyFlagsMap;
				static const std::unordered_map<std::string, vk::ImageAspectFlags> _vkImageAspectFlagBitsStringToVkImageAspectFlagsMap;
			public:
				static inline vk::BlendOp ParseToVkBlendOp(const std::string& type)
				{
					return _vkBlendOpStringToVkBlendOpMap.at(type);
				}
				static inline vk::BlendFactor ParseToVkBlendFactor(const std::string& type)
				{
					return _vkBlendFactorStringToVkBlendFactorMap.at(type);
				}
				static inline vk::ColorComponentFlags ParseToVkColorComponentFlags(const std::vector<std::string>& types)
				{
					vk::ColorComponentFlags flag{};
					for (const auto& type : types)
					{
						flag |= _vkColorComponentFlagBitsStringToVkColorComponentFlagBitsMap.at(type);
					}
					return flag;
				}
				static inline vk::CullModeFlags ParseToVkCullModeFlags(std::string type)
				{
					return _vkCullModeFlagBitsStringToVkCullModeFlagBitsMap.at(type);
				}
				static inline vk::CompareOp ParseToVkCompareOp(const std::string& type)
				{
					return _vkCompareOpStringToVkCompareOpMap.at(type);
				}
				static inline vk::FrontFace ParseToVkFrontFace(const std::string& type)
				{
					if (type == "COUNTER_CLOCKWISE") return vk::FrontFace::eCounterClockwise;
					else if (type == "CLOCKWISE") return vk::FrontFace::eClockwise;
					else return vk::FrontFace();
				}
				static inline vk::ImageLayout ParseToVkImageLayout(const std::string& type)
				{
					return _vkImageLayoutStringToVkImageLayoutMap.at(type);
				}
				inline static vk::Format ParseToVkFormat(const std::string& formatString)
				{
					return vk::Format(_vkFormatStringToVkFormatMap.at(formatString));
				}
				inline static vk::ImageUsageFlags ParseToVkImageUsageFlagBits(const std::string& usageString)
				{
					return _vkImageUsageFlagBitsStringToVkImageUsageFlagsMap.at(usageString);
				}
				inline static vk::MemoryPropertyFlags ParseToVkMemoryPropertyFlagBits(const std::string& propertyString)
				{
					return _vkMemoryPropertyFlagBitsStringToVkMemoryPropertyFlagsMap.at(propertyString);
				}
				inline static vk::ImageAspectFlags ParseToVkImageAspectFlagBits(const std::string& propertyString)
				{
					return _vkImageAspectFlagBitsStringToVkImageAspectFlagsMap.at(propertyString);
				}
			};
		}
	}
}