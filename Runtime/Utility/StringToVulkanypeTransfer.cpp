#include "StringToVulkanypeTransfer.hpp"

const std::unordered_map<std::string, vk::BlendOp> AirEngine::Runtime::Utility::StringToVulkanypeTransfer::_vkBlendOpStringToVkBlendOpMap
{
	{ "ADD", vk::BlendOp::eAdd },
	{ "SUBTRACT", vk::BlendOp::eSubtract },
	{ "REVERSE_SUBTRACT", vk::BlendOp::eReverseSubtract },
	{ "MIN", vk::BlendOp::eMin },
	{ "MAX", vk::BlendOp::eMax },
};

const std::unordered_map<std::string, vk::BlendFactor> AirEngine::Runtime::Utility::StringToVulkanypeTransfer::_vkBlendFactorStringToVkBlendFactorMap
{
	{ "ZERO", vk::BlendFactor(VK_BLEND_FACTOR_ZERO) },
	{ "ONE", vk::BlendFactor(VK_BLEND_FACTOR_ONE) },
	{ "SRC_COLOR", vk::BlendFactor(VK_BLEND_FACTOR_SRC_COLOR) },
	{ "ONE_MINUS_SRC_COLOR", vk::BlendFactor(VK_BLEND_FACTOR_ONE_MINUS_SRC_COLOR) },
	{ "DST_COLOR", vk::BlendFactor(VK_BLEND_FACTOR_DST_COLOR) },
	{ "ONE_MINUS_DST_COLOR", vk::BlendFactor(VK_BLEND_FACTOR_ONE_MINUS_DST_COLOR) },
	{ "SRC_ALPHA", vk::BlendFactor(VK_BLEND_FACTOR_SRC_ALPHA) },
	{ "ONE_MINUS_SRC_ALPHA", vk::BlendFactor(VK_BLEND_FACTOR_ONE_MINUS_SRC_ALPHA) },
	{ "DST_ALPHA", vk::BlendFactor(VK_BLEND_FACTOR_DST_ALPHA) },
	{ "ONE_MINUS_DST_ALPHA", vk::BlendFactor(VK_BLEND_FACTOR_ONE_MINUS_DST_ALPHA) },
	{ "CONSTANT_COLOR", vk::BlendFactor(VK_BLEND_FACTOR_CONSTANT_COLOR) },
	{ "ONE_MINUS_CONSTANT_COLOR", vk::BlendFactor(VK_BLEND_FACTOR_ONE_MINUS_CONSTANT_COLOR) },
	{ "CONSTANT_ALPHA", vk::BlendFactor(VK_BLEND_FACTOR_CONSTANT_ALPHA) },
	{ "ONE_MINUS_CONSTANT_ALPHA", vk::BlendFactor(VK_BLEND_FACTOR_ONE_MINUS_CONSTANT_ALPHA) },
	{ "SRC_ALPHA_SATURATE", vk::BlendFactor(VK_BLEND_FACTOR_SRC_ALPHA_SATURATE) },
	{ "SRC1_COLOR", vk::BlendFactor(VK_BLEND_FACTOR_SRC1_COLOR) },
	{ "ONE_MINUS_SRC1_COLOR", vk::BlendFactor(VK_BLEND_FACTOR_ONE_MINUS_SRC1_COLOR) },
	{ "SRC1_ALPHA", vk::BlendFactor(VK_BLEND_FACTOR_SRC1_ALPHA) },
	{ "ONE_MINUS_SRC1_ALPHA", vk::BlendFactor(VK_BLEND_FACTOR_ONE_MINUS_SRC1_ALPHA) },
};
const std::unordered_map<std::string, vk::ColorComponentFlagBits> AirEngine::Runtime::Utility::StringToVulkanypeTransfer::_vkColorComponentFlagBitsStringToVkColorComponentFlagBitsMap
{
	{ "R", vk::ColorComponentFlagBits::eR },
	{ "G", vk::ColorComponentFlagBits::eG },
	{ "B", vk::ColorComponentFlagBits::eB },
	{ "A", vk::ColorComponentFlagBits::eA }
};
const std::unordered_map<std::string, vk::CullModeFlagBits> AirEngine::Runtime::Utility::StringToVulkanypeTransfer::_vkCullModeFlagBitsStringToVkCullModeFlagBitsMap
{
	{ "NONE", vk::CullModeFlagBits::eNone },
	{ "FRONT", vk::CullModeFlagBits::eFront },
	{ "BACK", vk::CullModeFlagBits::eBack },
	{ "FRONT_AND_BACK", vk::CullModeFlagBits::eFrontAndBack }
};
const std::unordered_map<std::string, vk::CompareOp> AirEngine::Runtime::Utility::StringToVulkanypeTransfer::_vkCompareOpStringToVkCompareOpMap
{
	{ "NEVER", vk::CompareOp::eNever },
	{ "LESS", vk::CompareOp::eLess },
	{ "EQUAL", vk::CompareOp::eEqual },
	{ "LESS_OR_EQUAL", vk::CompareOp::eLessOrEqual },
	{ "GREATER", vk::CompareOp::eGreater },
	{ "NOT_EQUAL", vk::CompareOp::eNotEqual },
	{ "GREATER_OR_EQUAL", vk::CompareOp::eGreaterOrEqual },
	{ "ALWAYS", vk::CompareOp::eAlways },
};
