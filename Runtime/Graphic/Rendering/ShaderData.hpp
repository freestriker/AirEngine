#pragma once
#include "../../Utility/InternedString.hpp"
#include <vulkan/vulkan.hpp>
#include <spirv_reflect.h>

namespace AirEngine
{
	namespace Runtime
	{
		namespace AssetLoader
		{
			class ShaderLoader;
		}
		namespace Graphic
		{
			namespace Instance
			{
				class RenderPassBase;
			}
			namespace Rendering
			{
				struct DescriptorSetInfo;
				struct SubShaderInfo;
				struct ShaderInfo;
				struct VertexInputInfo
				{
					AirEngine::Runtime::Utility::InternedString name;
					vk::Format format;
					uint32_t location;
				};
				struct DescriptorInfo
				{
					AirEngine::Runtime::Utility::InternedString name;
					uint8_t set;
					uint8_t binding;
					uint16_t index;
					vk::DescriptorType type;
					uint8_t descriptorCount;
					struct ImageInfo
					{
						using Dim = SpvDim;
						Dim dim;
						vk::Format format;
						bool isArray;
						uint32_t inputAttachmentIndex;
					};
					std::unique_ptr<ImageInfo> imageInfo;

					uint16_t startByteOffsetInDescriptorSet;
					uint16_t solidByteSizeInDescriptorSet;
					uint8_t singleDescriptorByteSize;

					const DescriptorSetInfo* descriptorSetInfo;
				};
				struct DescriptorSetInfo
				{
					uint8_t set;
					uint8_t index;
					std::vector<uint16_t> descriptorInfoIndexs;
					uint16_t solidByteSize;
					bool isDynamicByteSize;
					vk::DescriptorSetLayout layout;

					const SubShaderInfo* subShaderInfo;
				};
				struct PushConstantInfo
				{
					vk::ShaderStageFlags shaderStageFlags;
					uint32_t size;
					bool valid;
				};
				struct SubShaderInfo
				{
					AirEngine::Runtime::Utility::InternedString subPass;
					std::unordered_map< AirEngine::Runtime::Utility::InternedString, VertexInputInfo> nameToVertexInputInfoMap;
					std::vector<DescriptorInfo> descriptorInfos{};
					std::vector<DescriptorSetInfo> descriptorSetInfos{};
					std::unordered_map<AirEngine::Runtime::Utility::InternedString, uint16_t> descriptorNameToDescriptorInfoIndexMap{};
					std::unordered_map<uint8_t, uint8_t> setToDescriptorSetInfoIndexMap;
					vk::PipelineLayout pipelineLayout;
					vk::Pipeline pipeline;
					PushConstantInfo pushConstantInfo;

					const ShaderInfo* shaderInfo;
				};
				struct ShaderInfo
				{
					vk::PipelineBindPoint pipelineBindPoint;
					std::unordered_map<AirEngine::Runtime::Utility::InternedString, SubShaderInfo> subShaderInfoMap{};
					Instance::RenderPassBase* renderPass;
				};
			}
		}
	}
}