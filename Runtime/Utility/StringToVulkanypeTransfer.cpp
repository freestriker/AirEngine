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
const std::unordered_map<std::string, vk::ImageLayout> AirEngine::Runtime::Utility::StringToVulkanypeTransfer::_vkImageLayoutStringToVkCompareOpMap
{
	{"Undefined", vk::ImageLayout(VK_IMAGE_LAYOUT_UNDEFINED)},
	{"General", vk::ImageLayout(VK_IMAGE_LAYOUT_GENERAL)},
	{"ColorAttachmentOptimal", vk::ImageLayout(VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL)},
	{"DepthStencilAttachmentOptimal", vk::ImageLayout(VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL)},
	{"DepthStencilReadOnlyOptimal", vk::ImageLayout(VK_IMAGE_LAYOUT_DEPTH_STENCIL_READ_ONLY_OPTIMAL)},
	{"ShaderReadOnlyOptimal", vk::ImageLayout(VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL)},
	{"TransferSrcOptimal", vk::ImageLayout(VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL)},
	{"TransferDstOptimal", vk::ImageLayout(VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL)},
	{"Preinitialized", vk::ImageLayout(VK_IMAGE_LAYOUT_PREINITIALIZED)},
	{"DepthReadOnlyStencilAttachmentOptimal", vk::ImageLayout(VK_IMAGE_LAYOUT_DEPTH_READ_ONLY_STENCIL_ATTACHMENT_OPTIMAL)},
	{"DepthAttachmentStencilReadOnlyOptimal", vk::ImageLayout(VK_IMAGE_LAYOUT_DEPTH_ATTACHMENT_STENCIL_READ_ONLY_OPTIMAL)},
	{"DepthAttachmentOptimal", vk::ImageLayout(VK_IMAGE_LAYOUT_DEPTH_ATTACHMENT_OPTIMAL)},
	{"DepthReadOnlyOptimal", vk::ImageLayout(VK_IMAGE_LAYOUT_DEPTH_READ_ONLY_OPTIMAL)},
	{"StencilAttachmentOptimal", vk::ImageLayout(VK_IMAGE_LAYOUT_STENCIL_ATTACHMENT_OPTIMAL)},
	{"StencilReadOnlyOptimal", vk::ImageLayout(VK_IMAGE_LAYOUT_STENCIL_READ_ONLY_OPTIMAL)},
	{"ReadOnlyOptimal", vk::ImageLayout(VK_IMAGE_LAYOUT_READ_ONLY_OPTIMAL)},
	{"AttachmentOptimal", vk::ImageLayout(VK_IMAGE_LAYOUT_ATTACHMENT_OPTIMAL)},
	{"PresentSrcKHR", vk::ImageLayout(VK_IMAGE_LAYOUT_PRESENT_SRC_KHR)},
	{"VideoDecodeDstKHR", vk::ImageLayout(VK_IMAGE_LAYOUT_VIDEO_DECODE_DST_KHR)},
	{"VideoDecodeSrcKHR", vk::ImageLayout(VK_IMAGE_LAYOUT_VIDEO_DECODE_SRC_KHR)},
	{"VideoDecodeDpbKHR", vk::ImageLayout(VK_IMAGE_LAYOUT_VIDEO_DECODE_DPB_KHR)},
	{"SharedPresentKHR", vk::ImageLayout(VK_IMAGE_LAYOUT_SHARED_PRESENT_KHR)},
	{"DepthReadOnlyStencilAttachmentOptimalKHR", vk::ImageLayout(VK_IMAGE_LAYOUT_DEPTH_READ_ONLY_STENCIL_ATTACHMENT_OPTIMAL_KHR)},
	{"DepthAttachmentStencilReadOnlyOptimalKHR", vk::ImageLayout(VK_IMAGE_LAYOUT_DEPTH_ATTACHMENT_STENCIL_READ_ONLY_OPTIMAL_KHR)},
	{"ShadingRateOptimalNV", vk::ImageLayout(VK_IMAGE_LAYOUT_SHADING_RATE_OPTIMAL_NV)},
	{"FragmentDensityMapOptimalEXT", vk::ImageLayout(VK_IMAGE_LAYOUT_FRAGMENT_DENSITY_MAP_OPTIMAL_EXT)},
	{"FragmentShadingRateAttachmentOptimalKHR", vk::ImageLayout(VK_IMAGE_LAYOUT_FRAGMENT_SHADING_RATE_ATTACHMENT_OPTIMAL_KHR)},
	{"DepthAttachmentOptimalKHR", vk::ImageLayout(VK_IMAGE_LAYOUT_DEPTH_ATTACHMENT_OPTIMAL_KHR)},
	{"DepthReadOnlyOptimalKHR", vk::ImageLayout(VK_IMAGE_LAYOUT_DEPTH_READ_ONLY_OPTIMAL_KHR)},
	{"StencilAttachmentOptimalKHR", vk::ImageLayout(VK_IMAGE_LAYOUT_STENCIL_ATTACHMENT_OPTIMAL_KHR)},
	{"StencilReadOnlyOptimalKHR", vk::ImageLayout(VK_IMAGE_LAYOUT_STENCIL_READ_ONLY_OPTIMAL_KHR)}
};
