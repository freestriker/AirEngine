﻿#pragma once
#include "ContructorMacro.hpp"
#include "ExportMacro.hpp"
#include <unordered_map>
#include <string>
#include <vulkan/vulkan_core.h>
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
			public:
				inline vk::BlendOp ParseToVkBlendOp(const std::string& type) const
				{
					return _vkBlendOpStringToVkBlendOpMap.at(type);
				}
				vk::BlendFactor ParseToVkBlendFactor(const std::string& type)
				{
					return _vkBlendFactorStringToVkBlendFactorMap.at(type);
				}
				vk::ColorComponentFlags ParseToVkColorComponentFlags(const std::vector<std::string>& types)
				{
					vk::ColorComponentFlags flag{};
					for (const auto& type : types)
					{
						flag |= _vkColorComponentFlagBitsStringToVkColorComponentFlagBitsMap.at(type);
					}
					return flag;
				}
				vk::CullModeFlags ParseToVkCullModeFlags(const std::vector<std::string>& types)
				{
					vk::CullModeFlags flag{};
					for (const auto& type : types)
					{
						flag |= _vkCullModeFlagBitsStringToVkCullModeFlagBitsMap.at(type);
					}
					return flag;
				}
				vk::CompareOp ParseToVkCompareOp(const std::string& type)
				{
					return _vkCompareOpStringToVkCompareOpMap.at(type);
				}
			};
		}
	}
}