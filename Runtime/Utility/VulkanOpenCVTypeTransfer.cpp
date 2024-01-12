#include "VulkanOpenCVTypeTransfer.hpp"
#include <vulkan/vk_enum_string_helper.h>

const std::unordered_map<std::string, AirEngine::Runtime::Utility::VulkanOpenCVTypeTransfer::PerChannelValueTypeIndex> AirEngine::Runtime::Utility::VulkanOpenCVTypeTransfer::_numericFormatStringToPerChannelValueTypeIndexMap
{
	{ "UNORM", UNVALID_PER_CHANNEL_VALUE_TYPE_INDEX },
	{ "SNORM", SIGNED_PER_CHANNEL_VALUE_TYPE_INDEX },
	{ "USCALED", UNVALID_PER_CHANNEL_VALUE_TYPE_INDEX },
	{ "SSCALED", SIGNED_PER_CHANNEL_VALUE_TYPE_INDEX },
	{ "UINT", UNVALID_PER_CHANNEL_VALUE_TYPE_INDEX },
	{ "SINT", SIGNED_PER_CHANNEL_VALUE_TYPE_INDEX },
	{ "SRGB", UNVALID_PER_CHANNEL_VALUE_TYPE_INDEX },
	{ "UFLOAT", UNVALID_PER_CHANNEL_VALUE_TYPE_INDEX },
	{ "SFLOAT", FLOAT_PER_CHANNEL_VALUE_TYPE_INDEX }
};

const int AirEngine::Runtime::Utility::VulkanOpenCVTypeTransfer::_perChannelBitsIndexAndPerChannelValueTypeIndexToPerChannelValueTypeMap[3][3]
{
	CV_8S, CV_8U, -1, 
	CV_16S, CV_16U, CV_16F,
	CV_32S, -1, CV_32F
};