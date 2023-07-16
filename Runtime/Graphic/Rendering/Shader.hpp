﻿#pragma once
#include "../../Asset/AssetBase.hpp"
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
			namespace Rendering
			{
				class AIR_ENGINE_API Shader final
					: public Asset::AssetBase
				{
					friend class AssetLoader::ShaderLoader;
				public:
					struct DescriptorSetInfo;
					struct SubShaderInfo;
					struct ShaderInfo;
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
						uint16_t index;
						std::vector<uint8_t> descriptorInfoIndexs;
						uint16_t solidByteSize;
						bool isDynamicByteSize;
						vk::DescriptorSetLayout layout;

						const SubShaderInfo* subShaderInfo;
					};
					struct SubShaderInfo
					{
						AirEngine::Runtime::Utility::InternedString subPass;
						std::vector<DescriptorInfo> descriptorInfos{};
						std::vector<DescriptorSetInfo> descriptorSetInfos{};
						std::unordered_map<AirEngine::Runtime::Utility::InternedString, uint16_t> descriptorNameToDescriptorInfoIndexMap{};

						const ShaderInfo* shaderInfo;
					};
					struct ShaderInfo
					{
						std::unordered_map<AirEngine::Runtime::Utility::InternedString, SubShaderInfo> subShaderInfoMap{};
					};
				private:
					ShaderInfo _shaderInfo;
				public:
					Shader();
					~Shader();
					NO_COPY_MOVE(Shader);
				};
			}
		}
	}
}