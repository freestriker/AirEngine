#include "ShaderLoader.hpp"
#include <fstream>
#include "AirEngine/Runtime/Core/Manager/GraphicDeviceManager.hpp"
#include "AirEngine/Runtime/Graphic/Rendering/Shader.hpp"
#include "AirEngine/Runtime/Utility/InternedString.hpp"
#include <spirv_reflect.h>
#include <unordered_map>
#include "AirEngine/Runtime/Graphic/Instance/RenderPassBase.hpp"
#include "AirEngine/Runtime/Graphic/Manager/RenderPassManager.hpp"
#include "AirEngine/Runtime/Core/Manager/GraphicDeviceManager.hpp"
#include "AirEngine/Runtime/Graphic/Manager/ShaderManager.hpp"
#include <vulkan/vulkan.hpp>
#include "AirEngine/Runtime/Utility/StringToVulkanypeTransfer.hpp"
#include "AirEngine/Runtime/Core/Manager/TaskManager.hpp"

AirEngine::Runtime::Asset::AssetBase* AirEngine::Runtime::AssetLoader::ShaderLoader::OnLoadAsset(const std::string& path, std::shared_future<void>& loadOperationFuture, bool& isInLoading)
{
	auto&& shader = new Graphic::Rendering::Shader();
	bool* isLoadingPtr = &isInLoading;

	loadOperationFuture = std::move(
		std::shared_future<void>(
			std::move(
				Core::Manager::TaskManager::Executor().async(
					[shader, path, isLoadingPtr]()->void
					{
						PopulateShader(shader, path, isLoadingPtr);
					}
				)
			)
		)
	);

	return shader;
}

void AirEngine::Runtime::AssetLoader::ShaderLoader::OnUnloadAsset(AirEngine::Runtime::Asset::AssetBase* asset)
{
	delete static_cast<AirEngine::Runtime::Graphic::Rendering::Shader*>(asset);
}

struct ColorAttachmentBlendStateDescriptor
{
	std::string colorAttachment;

	bool blendEnable{};
	std::string srcColorBlendFactor{};
	std::string colorBlendOp{};
	std::string dstColorBlendFactor{};
	std::string srcAlphaBlendFactor{};
	std::string alphaBlendOp{};
	std::string dstAlphaBlendFactor{};

	std::vector<std::string> colorWriteMasks{};

	NLOHMANN_DEFINE_TYPE_INTRUSIVE
	(
		ColorAttachmentBlendStateDescriptor,

		colorAttachment,

		blendEnable,
		srcColorBlendFactor,
		colorBlendOp,
		dstColorBlendFactor,
		srcAlphaBlendFactor,
		alphaBlendOp,
		dstAlphaBlendFactor,

		colorWriteMasks
	);
};
struct SubShaderDescriptor
{
	std::string subPass{};

	std::vector<std::string> spvShaderPaths{};

	std::vector <std::string> cullModes;
	std::string frountFace;

	bool depthTestEnable{};
	bool depthWriteEnable{};
	std::string depthCompareOp{};

	std::vector<ColorAttachmentBlendStateDescriptor> colorAttachmentBlendStates{};

	NLOHMANN_DEFINE_TYPE_INTRUSIVE
	(
		SubShaderDescriptor,

		subPass,

		spvShaderPaths,

		cullModes,
		frountFace,

		depthTestEnable,
		depthWriteEnable,
		depthCompareOp,

		colorAttachmentBlendStates
	);
};
struct ShaderDescriptor
{
	std::string renderPass{};

	std::vector<SubShaderDescriptor> subShaders;

	NLOHMANN_DEFINE_TYPE_INTRUSIVE
	(
		ShaderDescriptor,

		renderPass,

		subShaders
	);
};
struct SubShaderCreateInfo
{
	std::unordered_map<vk::ShaderStageFlagBits, std::pair<SpvReflectShaderModule, vk::ShaderModule>> shaderDatas;
	std::optional<vk::PushConstantRange> pushConstantRange;
};
struct ShaderCreateInfo
{
	std::unordered_map<std::string, SubShaderCreateInfo> subShaderCreateInfos;
	AirEngine::Runtime::Graphic::Instance::RenderPassBase* renderPass;
};

void LoadSpirvData(AirEngine::Runtime::Graphic::Rendering::ShaderInfo& shaderInfo, const ShaderDescriptor& shaderDescriptor, ShaderCreateInfo& shaderCreateInfo)
{
	vk::ShaderStageFlags stages{};
	for (const auto& subShaderDescriptor : shaderDescriptor.subShaders)
	{
		auto&& subShaderCreateInfo = shaderCreateInfo.subShaderCreateInfos[subShaderDescriptor.subPass];

		for (const auto& spirvPath : subShaderDescriptor.spvShaderPaths)
		{
			std::ifstream file(spirvPath, std::ios::ate | std::ios::binary);

			if (!file.is_open()) qFatal(std::string("Failed to open spv file: " + spirvPath + " .").c_str());

			size_t fileSize = (size_t)file.tellg();
			std::vector<char> buffer(fileSize);
			file.seekg(0);
			file.read(buffer.data(), fileSize);
			file.close();

			std::pair<SpvReflectShaderModule, vk::ShaderModule> shaderData{};

			SpvReflectResult result = spvReflectCreateShaderModule(buffer.size(), buffer.data(), &shaderData.first);
			if (result != SpvReflectResult::SPV_REFLECT_RESULT_SUCCESS) qFatal("Load shader spv reflect failed.");

			vk::ShaderModuleCreateInfo shaderModuleCreateInfo{vk::ShaderModuleCreateFlags(), buffer.size(), reinterpret_cast<const uint32_t*>(buffer.data())};
			shaderData.second = AirEngine::Runtime::Core::Manager::GraphicDeviceManager::Device().createShaderModule(shaderModuleCreateInfo);

			auto stage = vk::ShaderStageFlagBits(shaderData.first.shader_stage);
			if(subShaderCreateInfo.shaderDatas.contains(stage)) qFatal("Failed to load the same shader stage.");
			stages |= stage;
			
			subShaderCreateInfo.shaderDatas[stage] = std::move(shaderData);
		}
	}
	if (stages & vk::ShaderStageFlagBits::eVertex && stages & vk::ShaderStageFlagBits::eFragment)
	{
		shaderInfo.pipelineBindPoint = vk::PipelineBindPoint::eGraphics;
	}
	else
	{
		qFatal("Failed to get right pipeline bind point.");
	}
}

void LoadRenderPassData(AirEngine::Runtime::Graphic::Rendering::ShaderInfo& shaderInfo, const ShaderDescriptor& shaderDescriptor, ShaderCreateInfo& shaderCreateInfo)
{
	shaderCreateInfo.renderPass = AirEngine::Runtime::Graphic::Manager::RenderPassManager::LoadRenderPass(shaderDescriptor.renderPass);
	shaderInfo.renderPass = shaderCreateInfo.renderPass;
}

void CheckGraphicShaderStageInOutData(AirEngine::Runtime::Graphic::Rendering::ShaderInfo& shaderInfo, const ShaderDescriptor& shaderDescriptor, ShaderCreateInfo& shaderCreateInfo)
{
	constexpr auto GRAPHIC_SHADER_STAGES{ 
		std::to_array<vk::ShaderStageFlagBits>(
		{ 
			vk::ShaderStageFlagBits::eVertex,
			//vk::ShaderStageFlagBits::eTessellationControl,
			//vk::ShaderStageFlagBits::eTessellationEvaluation,
			//vk::ShaderStageFlagBits::eGeometry,
			vk::ShaderStageFlagBits::eFragment,
		}) 
	};
	for (const auto& subShaderDescriptor : shaderDescriptor.subShaders)
	{
		const auto& subShaderCreateInfo = shaderCreateInfo.subShaderCreateInfos.at(subShaderDescriptor.subPass);

		std::map<uint32_t, SpvReflectInterfaceVariable*> preOutputLocationToVariableMap{};
		
		//vertex
		{
			const auto& vertexShaderReflectData = subShaderCreateInfo.shaderDatas.at(vk::ShaderStageFlagBits::eVertex).first;

			uint32_t variableCount = 0;
			SpvReflectResult result = spvReflectEnumerateOutputVariables(&vertexShaderReflectData, &variableCount, NULL);
			if (result != SpvReflectResult::SPV_REFLECT_RESULT_SUCCESS) qFatal("Failed to enumerate shader variables.");
			std::vector<SpvReflectInterfaceVariable*> temporaryVariables{variableCount};
			result = spvReflectEnumerateOutputVariables(&vertexShaderReflectData, &variableCount, temporaryVariables.data());
			if (result != SpvReflectResult::SPV_REFLECT_RESULT_SUCCESS) qFatal("Failed to enumerate shader variables.");

			for (const auto& variable : temporaryVariables)
			{
				auto&& name(std::string(variable->name));
				if (name != "" && !name.starts_with("gl_"))
				{
					preOutputLocationToVariableMap.emplace(variable->location, variable);
				}
			}
		}

		//other
		for (uint32_t stageIndex = 1; stageIndex < GRAPHIC_SHADER_STAGES.size() - 1; ++stageIndex)
		{
			auto&& stage = GRAPHIC_SHADER_STAGES.at(stageIndex);

			if (!subShaderCreateInfo.shaderDatas.contains(stage)) continue;

			const auto& shaderReflectData = subShaderCreateInfo.shaderDatas.at(stage).first;
			{
				uint32_t variableCount = 0;
				SpvReflectResult result = spvReflectEnumerateInputVariables(&shaderReflectData, &variableCount, NULL);
				if (result != SpvReflectResult::SPV_REFLECT_RESULT_SUCCESS) qFatal("Failed to enumerate shader variables.");
				std::vector<SpvReflectInterfaceVariable*> temporaryVariables{variableCount};
				result = spvReflectEnumerateInputVariables(&shaderReflectData, &variableCount, temporaryVariables.data());
				if (result != SpvReflectResult::SPV_REFLECT_RESULT_SUCCESS) qFatal("Failed to enumerate shader variables.");

				std::map<uint32_t, SpvReflectInterfaceVariable*> curInputLocationToVariableMap{};
				for (const auto& variable : temporaryVariables)
				{
					auto&& name(std::string(variable->name));
					if (name != "" && !name.starts_with("gl_"))
					{
						curInputLocationToVariableMap.emplace(variable->location, variable);
					}
				}

				if (curInputLocationToVariableMap.size() == preOutputLocationToVariableMap.size())
				{
					for (const auto& outPair : preOutputLocationToVariableMap)
					{
						const auto& outVariable = *outPair.second;
						auto&& inIter = curInputLocationToVariableMap.find(outPair.first);
						if (inIter != curInputLocationToVariableMap.end())
						{
							auto&& inPair = *inIter;
							const auto& inVariable = *inPair.second;
							if (outVariable.format == inVariable.format && std::strcmp(outVariable.name, inVariable.name) == 0)
							{
								continue;
							}
							else
							{
								qFatal("Failed to match shader stage's input and ouput.");
							}
						}
						else
						{
							qFatal("Failed to match shader stage's input and ouput.");
						}
					}
				}
				else
				{
					qFatal("Failed to match shader stage's input and ouput.");
				}
			}
			{
				uint32_t variableCount = 0;
				SpvReflectResult result = spvReflectEnumerateOutputVariables(&shaderReflectData, &variableCount, NULL);
				if (result != SpvReflectResult::SPV_REFLECT_RESULT_SUCCESS) qFatal("Failed to enumerate shader variables.");
				std::vector<SpvReflectInterfaceVariable*> temporaryVariables{variableCount};
				result = spvReflectEnumerateOutputVariables(&shaderReflectData, &variableCount, temporaryVariables.data());
				if (result != SpvReflectResult::SPV_REFLECT_RESULT_SUCCESS) qFatal("Failed to enumerate shader variables.");

				std::map<uint32_t, SpvReflectInterfaceVariable*>& curOutputLocationToVariableMap{preOutputLocationToVariableMap};
				curOutputLocationToVariableMap.clear();
				for (const auto& variable : temporaryVariables)
				{
					auto&& name(std::string(variable->name));
					if (name != "" && !name.starts_with("gl_"))
					{
						curOutputLocationToVariableMap.emplace(variable->location, variable);
					}
				}
			}
		}

		//frag
		{
			const auto& fragShaderReflectData = subShaderCreateInfo.shaderDatas.at(vk::ShaderStageFlagBits::eFragment).first;

			uint32_t variableCount = 0;
			SpvReflectResult result = spvReflectEnumerateInputVariables(&fragShaderReflectData, &variableCount, NULL);
			if (result != SpvReflectResult::SPV_REFLECT_RESULT_SUCCESS) qFatal("Failed to enumerate shader variables.");
			std::vector<SpvReflectInterfaceVariable*> temporaryVariables{variableCount};
			result = spvReflectEnumerateInputVariables(&fragShaderReflectData, &variableCount, temporaryVariables.data());
			if (result != SpvReflectResult::SPV_REFLECT_RESULT_SUCCESS) qFatal("Failed to enumerate shader variables.");

			std::map<uint32_t, SpvReflectInterfaceVariable*> curInputLocationToVariableMap{};
			for (const auto& variable : temporaryVariables)
			{
				auto&& name(std::string(variable->name));
				if (name != "" && !name.starts_with("gl_"))
				{
					curInputLocationToVariableMap.emplace(variable->location, variable);
				}
			}

			if (curInputLocationToVariableMap.size() == preOutputLocationToVariableMap.size())
			{
				for (const auto& outPair : preOutputLocationToVariableMap)
				{
					const auto& outVariable = *outPair.second;
					auto&& inIter = curInputLocationToVariableMap.find(outPair.first);
					if (inIter != curInputLocationToVariableMap.end())
					{
						const auto& inVariable = *(*inIter).second;
						if (outVariable.format == inVariable.format && std::strcmp(outVariable.name, inVariable.name) == 0)
						{
							continue;
						}
						else
						{
							qFatal("Failed to match shader stage's input and ouput.");
						}
					}
					else
					{
						qFatal("Failed to match shader stage's input and ouput.");
					}
				}
			}
			else
			{
				qFatal("Failed to match shader stage's input and ouput.");
			}
		}
	}
}

void ParseShaderInfo(AirEngine::Runtime::Graphic::Rendering::ShaderInfo& shaderInfo, const ShaderDescriptor& shaderDescriptor, ShaderCreateInfo& shaderCreateInfo)
{
	using namespace AirEngine::Runtime::Graphic::Rendering;

	std::unordered_map<AirEngine::Runtime::Utility::InternedString, std::tuple<uint16_t, uint8_t>> descriptorInfoNameToStartIndexAndCountMap{};

	for (const auto& subShaderDescriptor : shaderDescriptor.subShaders)
	{
		auto& subShaderCreateInfo = shaderCreateInfo.subShaderCreateInfos.at(subShaderDescriptor.subPass);

		auto subPassName = AirEngine::Runtime::Utility::InternedString(subShaderDescriptor.subPass);
		auto&& subPassInfo = shaderCreateInfo.renderPass->Info().SubPassInfo(subPassName);

		std::map<uint8_t, std::map<uint8_t, vk::DescriptorSetLayoutBinding>> setToBindingToDescriptorBindingMap{};
		std::map<uint8_t, std::pair<DescriptorSetInfo, std::map<uint8_t, DescriptorInfo>>> setToDescriptorSetInfoAndBindingToDescriptorInfoMap{};

		//parse direct data
		for (const auto& shaderReflectDataPair : subShaderCreateInfo.shaderDatas)
		{
			const auto& shaderReflectData = shaderReflectDataPair.second.first;

			///push_constant
			{
				uint32_t pushConstantCount = 0;
				SpvReflectResult result = spvReflectEnumeratePushConstantBlocks(&shaderReflectData, &pushConstantCount, nullptr);
				assert(result == SPV_REFLECT_RESULT_SUCCESS);
				std::vector< SpvReflectBlockVariable*> blockVariables = std::vector< SpvReflectBlockVariable*>(pushConstantCount, nullptr);
				result = spvReflectEnumeratePushConstantBlocks(&shaderReflectData, &pushConstantCount, blockVariables.data());
				assert(result == SPV_REFLECT_RESULT_SUCCESS);

				if (pushConstantCount > 0)
				{
					if (pushConstantCount > 1) qFatal("Can not have multiple push constant block in one shader stage.");

					const auto& blockVariable = *blockVariables.front();
					vk::PushConstantRange range(vk::ShaderStageFlagBits(shaderReflectData.shader_stage), 0, blockVariable.size);

					if (subShaderCreateInfo.pushConstantRange)
					{
						subShaderCreateInfo.pushConstantRange->size = std::max(subShaderCreateInfo.pushConstantRange->size, range.size);
						subShaderCreateInfo.pushConstantRange->stageFlags |= range.stageFlags;
					}
					else
					{
						subShaderCreateInfo.pushConstantRange = range;
					}
				}
			}

			uint32_t descriptorSetCount = 0;
			SpvReflectResult result = spvReflectEnumerateDescriptorSets(&shaderReflectData, &descriptorSetCount, NULL);
			if (result != SPV_REFLECT_RESULT_SUCCESS) qFatal("Failed to enumerate descriptor sets.");
			std::vector<SpvReflectDescriptorSet*> reflectDescriptorSets(descriptorSetCount);
			result = spvReflectEnumerateDescriptorSets(&shaderReflectData, &descriptorSetCount, reflectDescriptorSets.data());
			if (result != SPV_REFLECT_RESULT_SUCCESS) qFatal("Failed to enumerate descriptor sets.");

			for (uint32_t setIndex = 0; setIndex < descriptorSetCount; ++setIndex)
			{
				const SpvReflectDescriptorSet& reflectDescriptorSet = *(reflectDescriptorSets[setIndex]);

				auto&& bindingToDescriptorBindingMap = setToBindingToDescriptorBindingMap[reflectDescriptorSet.set];

				auto&& descriptorSetInfo = setToDescriptorSetInfoAndBindingToDescriptorInfoMap[reflectDescriptorSet.set].first;
				descriptorSetInfo.set = reflectDescriptorSet.set;
				auto&& bindingToDescriptorInfoMap = setToDescriptorSetInfoAndBindingToDescriptorInfoMap[reflectDescriptorSet.set].second;

				for (uint32_t bindingIndex = 0; bindingIndex < reflectDescriptorSet.binding_count; ++bindingIndex)
				{
					const SpvReflectDescriptorBinding& reflectDescriptorBinding = *(reflectDescriptorSet.bindings[bindingIndex]);

					const auto neverParsed = !bindingToDescriptorBindingMap.contains(reflectDescriptorBinding.binding);

					//descriptorBinding
					auto&& descriptorBinding = bindingToDescriptorBindingMap[reflectDescriptorBinding.binding];
					{
						descriptorBinding.stageFlags |= vk::ShaderStageFlagBits(shaderReflectData.shader_stage);
						if (neverParsed)
						{
							descriptorBinding.binding = reflectDescriptorBinding.binding;
							descriptorBinding.descriptorType = vk::DescriptorType(reflectDescriptorBinding.descriptor_type);
							descriptorBinding.descriptorCount = 1;
							for (uint32_t dimeIndex = 0; dimeIndex < reflectDescriptorBinding.array.dims_count; ++dimeIndex)
							{
								descriptorBinding.descriptorCount *= reflectDescriptorBinding.array.dims[dimeIndex];
							}
						}
						else
						{
							continue;
						}
					}

					//descriptorInfo
					auto&& descriptorInfo = bindingToDescriptorInfoMap[reflectDescriptorBinding.binding];
					{
						descriptorInfo.name = AirEngine::Runtime::Utility::InternedString(reflectDescriptorBinding.name);
						descriptorInfo.set = reflectDescriptorSet.set;
						descriptorInfo.binding = reflectDescriptorBinding.binding;
						descriptorInfo.type = vk::DescriptorType(reflectDescriptorBinding.descriptor_type);
						descriptorInfo.descriptorCount = descriptorBinding.descriptorCount;
						if (
							descriptorInfo.type == vk::DescriptorType::eInputAttachment ||
							descriptorInfo.type == vk::DescriptorType::eCombinedImageSampler ||
							descriptorInfo.type == vk::DescriptorType::eStorageImage
							)
						{
							descriptorInfo.imageInfo = std::make_unique<DescriptorInfo::ImageInfo>
							(
								reflectDescriptorBinding.image.dim,
								vk::Format(reflectDescriptorBinding.image.image_format),
								reflectDescriptorBinding.image.arrayed != 0,
								reflectDescriptorBinding.input_attachment_index
							);
						}
					}
				}
			}
		}

		//Check binding
		for (const auto& perBindingDescriptorMapPair : setToBindingToDescriptorBindingMap)
		{
			const auto& set = perBindingDescriptorMapPair.first;
			const auto& perBindingDescriptorMap = perBindingDescriptorMapPair.second;

			uint32_t bindingCount = 0;
			std::unordered_set<AirEngine::Runtime::Utility::InternedString> descriptorNameSet{};
			for (const auto& descriptorPair : perBindingDescriptorMap)
			{
				const auto& binding = descriptorPair.first;
				const auto& descriptor = descriptorPair.second;
				const auto& descriptorInfo = setToDescriptorSetInfoAndBindingToDescriptorInfoMap.at(set).second.at(binding);

				if (bindingCount != binding)
				{
					qFatal("Wrong binding in shader.");
				}
				else
				{
					bindingCount += descriptor.descriptorCount;
				}

				if (descriptorNameSet.contains(descriptorInfo.name))
				{
					qFatal("Same descriptor name in shader.");
				}
				else
				{
					descriptorNameSet.insert(descriptorInfo.name);
				}

			}
		}

		//Check input
		{
			for (auto& setToDescriptorSetInfoAndBindingToDescriptorInfoMapPair : setToDescriptorSetInfoAndBindingToDescriptorInfoMap)
			{
				const auto& set = setToDescriptorSetInfoAndBindingToDescriptorInfoMapPair.first;
				auto&& bindingToDescriptorInfoMap = setToDescriptorSetInfoAndBindingToDescriptorInfoMapPair.second.second;
				auto&& descriptorSetInfo = setToDescriptorSetInfoAndBindingToDescriptorInfoMapPair.second.first;

				for (auto& bindingToDescriptorInfoMapPair : bindingToDescriptorInfoMap)
				{
					const auto& binding = bindingToDescriptorInfoMapPair.first;
					auto& descriptorInfo = bindingToDescriptorInfoMapPair.second;

					if (descriptorInfo.type == vk::DescriptorType::eInputAttachment)
					{
						auto&& attachmentInfo = subPassInfo.AttachmentInfo(descriptorInfo.name);
						if (descriptorInfo.imageInfo->inputAttachmentIndex != attachmentInfo.location)
						{
							qFatal("Input attachment do not match.");
						}
					}
				}
			}
		}

		//create layout and size
		for (const auto& bindingToDescriptorBindingMapPair : setToBindingToDescriptorBindingMap)
		{
			const auto& set = bindingToDescriptorBindingMapPair.first;
			const auto& bindingToDescriptorBindingMap = bindingToDescriptorBindingMapPair.second;

			std::vector<vk::DescriptorSetLayoutBinding> vkDescriptorSetLayoutBindings{};
			for (const auto& bindingToDescriptorBindingMapPair : bindingToDescriptorBindingMap)
			{
				const auto& descriptorBinding = bindingToDescriptorBindingMapPair.second;

				vkDescriptorSetLayoutBindings.emplace_back(descriptorBinding);
			}

			const bool isDynamic = setToDescriptorSetInfoAndBindingToDescriptorInfoMap.at(set).second.rbegin()->second.descriptorCount == 0;

			std::vector<vk::DescriptorBindingFlags> flags{bindingToDescriptorBindingMap.size(), vk::DescriptorBindingFlags()};
			vk::DescriptorSetLayoutBindingFlagsCreateInfo setLayoutBindingFlags{flags};
			{
				if (isDynamic)
				{
					flags.back() = vk::DescriptorBindingFlagBits::eVariableDescriptorCount;
					vkDescriptorSetLayoutBindings.back().descriptorCount = 4096;
				}
			}

			vk::DescriptorSetLayoutCreateInfo descriptorSetLayoutCreateInfo(
				vk::DescriptorSetLayoutCreateFlagBits::eDescriptorBufferEXT,
				vkDescriptorSetLayoutBindings,
				&setLayoutBindingFlags
			);

			auto&& layout = AirEngine::Runtime::Core::Manager::GraphicDeviceManager::Device().createDescriptorSetLayout(descriptorSetLayoutCreateInfo);

			for(auto& bindingToDescriptorInfoMapPair: setToDescriptorSetInfoAndBindingToDescriptorInfoMap.at(set).second)
			{
				const auto binding = bindingToDescriptorInfoMapPair.first;
				auto& descriptorInfo = bindingToDescriptorInfoMapPair.second;
				descriptorInfo.singleDescriptorByteSize = AirEngine::Runtime::Graphic::Manager::ShaderManager::DescriptorSize(descriptorInfo.type);
				descriptorInfo.solidByteSizeInDescriptorSet = descriptorInfo.singleDescriptorByteSize * descriptorInfo.descriptorCount;
				descriptorInfo.startByteOffsetInDescriptorSet = uint16_t(AirEngine::Runtime::Core::Manager::GraphicDeviceManager::Device().getDescriptorSetLayoutBindingOffsetEXT(layout, uint32_t(binding)));
			}

			auto&& descriptorSetInfo = setToDescriptorSetInfoAndBindingToDescriptorInfoMap.at(set).first;
			descriptorSetInfo.layout = layout;
			descriptorSetInfo.isDynamicByteSize = isDynamic;
			descriptorSetInfo.solidByteSize = 
				isDynamic ? 
				setToDescriptorSetInfoAndBindingToDescriptorInfoMap.at(set).second.rbegin()->second.startByteOffsetInDescriptorSet: 
				uint16_t(AirEngine::Runtime::Core::Manager::GraphicDeviceManager::Device().getDescriptorSetLayoutSizeEXT(layout));
		}

		//compact object
		auto& subShaderInfo = shaderInfo.subShaderInfoMap[subPassName];
		subShaderInfo.subPass = subPassName;
		{
			if (subShaderCreateInfo.pushConstantRange)
			{
				subShaderInfo.pushConstantInfo.valid = true;
				subShaderInfo.pushConstantInfo.shaderStageFlags = subShaderCreateInfo.pushConstantRange->stageFlags;
				subShaderInfo.pushConstantInfo.size = subShaderCreateInfo.pushConstantRange->size;
			}
			else
			{
				subShaderInfo.pushConstantInfo.valid = false;
				subShaderInfo.pushConstantInfo.shaderStageFlags = {};
				subShaderInfo.pushConstantInfo.size = -1;
			}
		}
		for (auto& setToDescriptorSetInfoAndBindingToDescriptorInfoMapPair : setToDescriptorSetInfoAndBindingToDescriptorInfoMap)
		{
			const auto& set = setToDescriptorSetInfoAndBindingToDescriptorInfoMapPair.first;
			auto&& bindingToDescriptorInfoMap = setToDescriptorSetInfoAndBindingToDescriptorInfoMapPair.second.second;
			auto&& descriptorSetInfo = setToDescriptorSetInfoAndBindingToDescriptorInfoMapPair.second.first;

			descriptorSetInfo.index = subShaderInfo.descriptorSetInfos.size();

			subShaderInfo.setToDescriptorSetInfoIndexMap.emplace(descriptorSetInfo.set, uint8_t(subShaderInfo.descriptorSetInfos.size()));
			subShaderInfo.descriptorSetInfos.emplace_back(descriptorSetInfo);

			for (auto& bindingToDescriptorInfoMapPair : bindingToDescriptorInfoMap)
			{
				const auto& binding = bindingToDescriptorInfoMapPair.first;
				auto& descriptorInfo = bindingToDescriptorInfoMapPair.second;

				descriptorInfo.index = subShaderInfo.descriptorInfos.size();
				descriptorInfo.descriptorSetInfo = reinterpret_cast<DescriptorSetInfo*>(descriptorSetInfo.index);

				subShaderInfo.descriptorInfos.emplace_back(std::move(descriptorInfo));
				subShaderInfo.descriptorNameToDescriptorInfoIndexMap.emplace(std::pair{descriptorInfo.name, descriptorInfo.index});
			}
		}
	}

	//set parent ptr
	for (auto& subShaderInfoMapPair : shaderInfo.subShaderInfoMap)
	{
		auto&& subPassName = subShaderInfoMapPair.first;
		auto& subShaderInfo = subShaderInfoMapPair.second;

		subShaderInfo.shaderInfo = &shaderInfo;

		for (auto& descriptorSetInfo : subShaderInfo.descriptorSetInfos)
		{
			descriptorSetInfo.subShaderInfo = &subShaderInfo;
		}

		for (auto& descriptorInfo : subShaderInfo.descriptorInfos)
		{
			auto descriptorSetInfo = subShaderInfo.descriptorSetInfos.data() + reinterpret_cast<size_t>(descriptorInfo.descriptorSetInfo);
			descriptorSetInfo->descriptorInfoIndexs.emplace_back(descriptorInfo.index);
			descriptorInfo.descriptorSetInfo = reinterpret_cast<const DescriptorSetInfo*>(descriptorSetInfo);
		}
	}
}

void LoadVertexInputData(AirEngine::Runtime::Graphic::Rendering::ShaderInfo& shaderInfo, const ShaderDescriptor& shaderDescriptor, ShaderCreateInfo& shaderCreateInfo)
{
	for (auto& subShaderInfoMapPair : shaderInfo.subShaderInfoMap)
	{
		auto&& subPassName = subShaderInfoMapPair.first;
		auto&& subShaderInfo = subShaderInfoMapPair.second;

		const auto& vertexShaderReflectData = shaderCreateInfo.subShaderCreateInfos.at(subPassName.ToString()).shaderDatas.at(vk::ShaderStageFlagBits::eVertex).first;

		uint32_t inputCount = 0;
		SpvReflectResult result = spvReflectEnumerateInputVariables(&vertexShaderReflectData, &inputCount, NULL);
		if (result != SpvReflectResult::SPV_REFLECT_RESULT_SUCCESS) qFatal("Failed to enumerate shader input variables.");
		std::vector<SpvReflectInterfaceVariable*> input_vars(inputCount);
		result = spvReflectEnumerateInputVariables(&vertexShaderReflectData, &inputCount, input_vars.data());
		if (result != SpvReflectResult::SPV_REFLECT_RESULT_SUCCESS) qFatal("Failed to enumerate shader input variables.");

		for (uint32_t inputIndex = 0; inputIndex < inputCount; ++inputIndex)
		{
			const auto& inputVar = *input_vars[inputIndex];

			auto name = AirEngine::Runtime::Utility::InternedString::InternedString(inputVar.name);

			AirEngine::Runtime::Graphic::Rendering::VertexInputInfo vertexInputInfo{};
			vertexInputInfo.name = name;
			vertexInputInfo.location = inputVar.location;
			vertexInputInfo.format = vk::Format(inputVar.format);

			subShaderInfo.nameToVertexInputInfoMap.emplace(name, std::move(vertexInputInfo));
		}
	}
}

void CheckGraphicFragmentShaderOutData(AirEngine::Runtime::Graphic::Rendering::ShaderInfo& shaderInfo, const ShaderDescriptor& shaderDescriptor, ShaderCreateInfo& shaderCreateInfo)
{
	for (const auto& subShaderDescriptor : shaderDescriptor.subShaders)
	{
		const auto& subShaderCreateInfo = shaderCreateInfo.subShaderCreateInfos.at(subShaderDescriptor.subPass);
		auto&& subpassName = AirEngine::Runtime::Utility::InternedString(subShaderDescriptor.subPass);
		auto&& subpassInfo = shaderCreateInfo.renderPass->Info().SubPassInfo(subpassName);

		const auto& fragmentShaderReflectData = subShaderCreateInfo.shaderDatas.at(vk::ShaderStageFlagBits::eFragment).first;

		uint32_t variableCount = 0;
		SpvReflectResult result = spvReflectEnumerateOutputVariables(&fragmentShaderReflectData, &variableCount, NULL);
		if (result != SpvReflectResult::SPV_REFLECT_RESULT_SUCCESS) qFatal("Failed to enumerate shader variables.");
		std::vector<SpvReflectInterfaceVariable*> temporaryVariables{variableCount};
		result = spvReflectEnumerateOutputVariables(&fragmentShaderReflectData, &variableCount, temporaryVariables.data());
		if (result != SpvReflectResult::SPV_REFLECT_RESULT_SUCCESS) qFatal("Failed to enumerate shader variables.");

		for (const auto& variable : temporaryVariables)
		{
			auto&& attachmentInfo = subpassInfo.AttachmentInfo(AirEngine::Runtime::Utility::InternedString(variable->name));
			if (attachmentInfo.location != variable->location || attachmentInfo.type != AirEngine::Runtime::Graphic::Instance::RenderPassBase::AttachmentType::COLOR)
			{
				qFatal("Fragment output attachment do not match.");
			}
		}
	}
}
void CreateGraphicPipeline(AirEngine::Runtime::Graphic::Rendering::ShaderInfo& shaderInfo, const ShaderDescriptor& shaderDescriptor, ShaderCreateInfo& shaderCreateInfo)
{
	using namespace AirEngine::Runtime;

	constexpr auto DYNAMIC_STATES{
		std::to_array<vk::DynamicState>(
		{
			vk::DynamicState::eViewport,
			vk::DynamicState::eScissor,
			vk::DynamicState::eVertexInputEXT
		})
	};
	constexpr auto VIEWPORT{
		std::to_array<vk::Viewport>(
		{
			vk::Viewport(0, 0, 100, 100, 0, 1)
		})
	};
	constexpr auto SCISSOR{
		std::to_array<vk::Rect2D>(
		{
			vk::Rect2D({0, 0}, {100, 100})
		})
	};

	for (auto& subShaderInfoMapPair : shaderInfo.subShaderInfoMap)
	{
		auto&& subPassName = subShaderInfoMapPair.first;
		auto& subShaderInfo = subShaderInfoMapPair.second;
		auto&& setToDescriptorSetInfoIndexMap = subShaderInfo.setToDescriptorSetInfoIndexMap;
		auto&& subShaderDescriptor = *std::find_if(shaderDescriptor.subShaders.begin(), shaderDescriptor.subShaders.end(), [&](const SubShaderDescriptor& subShaderDescriptor)->bool {return subShaderDescriptor.subPass == subPassName.ToString(); });
		auto&& subShaderCreateInfo = shaderCreateInfo.subShaderCreateInfos.at(subPassName.ToString());
		const auto& subpassInfo = shaderCreateInfo.renderPass->Info().SubPassInfo(subPassName);

		vk::PipelineLayout pipelineLayout{};
		{
			std::vector<vk::DescriptorSetLayout> vkDescriptorSetLayouts{};
			for (auto& setToDescriptorSetInfoIndexMapPair : setToDescriptorSetInfoIndexMap)
			{
				auto&& set = setToDescriptorSetInfoIndexMapPair.first;
				auto&& descriptorSetInfoIndex = setToDescriptorSetInfoIndexMapPair.second;
				auto&& descriptorSetInfo = subShaderInfo.descriptorSetInfos[descriptorSetInfoIndex];

				vkDescriptorSetLayouts.emplace_back(descriptorSetInfo.layout);
			}

			vk::PipelineLayoutCreateInfo pipelineLayoutCreateInfo{vk::PipelineLayoutCreateFlags(), vkDescriptorSetLayouts};
			pipelineLayout = AirEngine::Runtime::Core::Manager::GraphicDeviceManager::Device().createPipelineLayout(pipelineLayoutCreateInfo);
		}

		vk::Pipeline pipeline{};
		{
			vk::PipelineViewportStateCreateInfo pipelineViewportStateCreateInfo
			{
				vk::PipelineViewportStateCreateFlags(),
					VIEWPORT,
					SCISSOR
			};
			vk::PipelineInputAssemblyStateCreateInfo pipelineInputAssemblyStateCreateInfo
			{
				vk::PipelineInputAssemblyStateCreateFlags(),
					vk::PrimitiveTopology::eTriangleList,
					VK_FALSE
			};
			vk::PipelineRasterizationStateCreateInfo pipelineRasterizationStateCreateInfo
			(
				vk::PipelineRasterizationStateCreateFlags(),
				VK_FALSE,
				VK_FALSE,
				vk::PolygonMode::eFill,
				Utility::StringToVulkanypeTransfer::ParseToVkCullModeFlags(subShaderDescriptor.cullModes),
				Utility::StringToVulkanypeTransfer::ParseToVkFrontFace(subShaderDescriptor.frountFace),
				VK_FALSE,
				0,
				0,
				0,
				1
			);

			vk::PipelineMultisampleStateCreateInfo pipelineMultisampleStateCreateInfo
			(
				vk::PipelineMultisampleStateCreateFlags(),
				vk::SampleCountFlagBits::e1,
				VK_FALSE
			);

			vk::PipelineDepthStencilStateCreateInfo pipelineDepthStencilStateCreateInfo
			(
				vk::PipelineDepthStencilStateCreateFlags(),
				subShaderDescriptor.depthTestEnable,
				subShaderDescriptor.depthWriteEnable,
				Utility::StringToVulkanypeTransfer::ParseToVkCompareOp(subShaderDescriptor.depthCompareOp)
			);
			std::vector<vk::PipelineColorBlendAttachmentState> vkPipelineColorBlendAttachmentStates{subShaderDescriptor.colorAttachmentBlendStates.size()};
			{
				for (const auto& colorAttachmentBlendStateDescriptor : subShaderDescriptor.colorAttachmentBlendStates)
				{
					auto&& attachmentName = colorAttachmentBlendStateDescriptor.colorAttachment;

					auto&& attachmentInfo = subpassInfo.AttachmentInfo(Utility::InternedString(attachmentName));
					if (attachmentInfo.type == AirEngine::Runtime::Graphic::Instance::RenderPassBase::AttachmentType::COLOR)
					{
						auto&& vkPipelineColorBlendAttachmentState = vkPipelineColorBlendAttachmentStates[attachmentInfo.location];
						vkPipelineColorBlendAttachmentState = vk::PipelineColorBlendAttachmentState
						(
							colorAttachmentBlendStateDescriptor.blendEnable,
							Utility::StringToVulkanypeTransfer::ParseToVkBlendFactor(colorAttachmentBlendStateDescriptor.srcColorBlendFactor),
							Utility::StringToVulkanypeTransfer::ParseToVkBlendFactor(colorAttachmentBlendStateDescriptor.dstColorBlendFactor),
							Utility::StringToVulkanypeTransfer::ParseToVkBlendOp(colorAttachmentBlendStateDescriptor.colorBlendOp),
							Utility::StringToVulkanypeTransfer::ParseToVkBlendFactor(colorAttachmentBlendStateDescriptor.srcAlphaBlendFactor),
							Utility::StringToVulkanypeTransfer::ParseToVkBlendFactor(colorAttachmentBlendStateDescriptor.dstAlphaBlendFactor),
							Utility::StringToVulkanypeTransfer::ParseToVkBlendOp(colorAttachmentBlendStateDescriptor.alphaBlendOp),
							Utility::StringToVulkanypeTransfer::ParseToVkColorComponentFlags(colorAttachmentBlendStateDescriptor.colorWriteMasks)
						);
					}
					else
					{
						qFatal("Blend state can only be used to color attachment.");
					}
				}
			}
			vk::PipelineColorBlendStateCreateInfo pipelineColorBlendStateCreateInfo
			(
				vk::PipelineColorBlendStateCreateFlags(),
				false,
				vk::LogicOp::eClear,
				vkPipelineColorBlendAttachmentStates
			);
			vk::PipelineDynamicStateCreateInfo pipelineDynamicStateCreateInfo
			(
				vk::PipelineDynamicStateCreateFlags(),
				DYNAMIC_STATES
			);
			std::vector<vk::PipelineShaderStageCreateInfo> pipelineShaderStageCreateInfos{};
			for (const auto& shaderDatasPair : subShaderCreateInfo.shaderDatas)
			{
				const auto& stage = shaderDatasPair.first;
				const auto& shaderReflectData = shaderDatasPair.second.first;
				const auto& shaderModule = shaderDatasPair.second.second;
				pipelineShaderStageCreateInfos.emplace_back(vk::PipelineShaderStageCreateInfo(vk::PipelineShaderStageCreateFlags(), stage, shaderModule, shaderReflectData.entry_point_name));
			}
			vk::GraphicsPipelineCreateInfo graphicsPipelineCreateInfo
			(
				vk::PipelineCreateFlags(),
				pipelineShaderStageCreateInfos,
				nullptr,
				&pipelineInputAssemblyStateCreateInfo,
				nullptr,
				&pipelineViewportStateCreateInfo,
				&pipelineRasterizationStateCreateInfo,
				&pipelineMultisampleStateCreateInfo,
				&pipelineDepthStencilStateCreateInfo,
				&pipelineColorBlendStateCreateInfo,
				&pipelineDynamicStateCreateInfo,
				pipelineLayout,
				shaderCreateInfo.renderPass->VkHandle(),
				shaderCreateInfo.renderPass->Info().SubPassInfo(subPassName).Index()
			);
			pipeline = AirEngine::Runtime::Core::Manager::GraphicDeviceManager::Device().createGraphicsPipeline({}, graphicsPipelineCreateInfo).value;
		}		

		subShaderInfo.pipelineLayout = pipelineLayout;
		subShaderInfo.pipeline = pipeline;
	}
}
void UnloadSpirvData(AirEngine::Runtime::Graphic::Rendering::ShaderInfo& shaderInfo, const ShaderDescriptor& shaderDescriptor, ShaderCreateInfo& shaderCreateInfo)
{
	for (auto& subShaderCreateInfoPair : shaderCreateInfo.subShaderCreateInfos)
	{
		auto&& shaderDatas = subShaderCreateInfoPair.second.shaderDatas;
		for (auto& shaderDatasPair : shaderDatas)
		{
			auto&& reflectShaderData = shaderDatasPair.second.first;
			auto&& shaderData = shaderDatasPair.second.second;

			spvReflectDestroyShaderModule(&reflectShaderData);
			AirEngine::Runtime::Core::Manager::GraphicDeviceManager::Device().destroyShaderModule(shaderData);
		}
	}
}

void AirEngine::Runtime::AssetLoader::ShaderLoader::PopulateShader(AirEngine::Runtime::Graphic::Rendering::Shader* shader, const std::string path, bool* isInLoading)
{
	//Load descriptor
	ShaderDescriptor shaderDescriptor{};
	{
		std::ifstream descriptorFile(path);
		if (!descriptorFile.is_open()) qFatal("Failed to open file.");
		nlohmann::json jsonFile = nlohmann::json::parse((std::istreambuf_iterator<char>(descriptorFile)), std::istreambuf_iterator<char>());
		shaderDescriptor = jsonFile.get<ShaderDescriptor>();
		descriptorFile.close();
	}

	ShaderCreateInfo shaderCreateInfo{};
	
	LoadSpirvData(shader->_shaderInfo, shaderDescriptor, shaderCreateInfo);
	switch (shader->_shaderInfo.pipelineBindPoint)
	{
		case vk::PipelineBindPoint::eGraphics:
		{
			LoadRenderPassData(shader->_shaderInfo, shaderDescriptor, shaderCreateInfo);
			CheckGraphicShaderStageInOutData(shader->_shaderInfo, shaderDescriptor, shaderCreateInfo);
			ParseShaderInfo(shader->_shaderInfo, shaderDescriptor, shaderCreateInfo);
			LoadVertexInputData(shader->_shaderInfo, shaderDescriptor, shaderCreateInfo);
			CheckGraphicFragmentShaderOutData(shader->_shaderInfo, shaderDescriptor, shaderCreateInfo);
			CreateGraphicPipeline(shader->_shaderInfo, shaderDescriptor, shaderCreateInfo);
			break;
		}
		default:
		{
			qFatal("Do not support this shader type.");
			break;
		}
	}
	UnloadSpirvData(shader->_shaderInfo, shaderDescriptor, shaderCreateInfo);

	*isInLoading = false;
}

AirEngine::Runtime::AssetLoader::ShaderLoader::ShaderLoader()
	: AssetLoaderBase("ShaderLoader", "shader")
{

}

AirEngine::Runtime::AssetLoader::ShaderLoader::~ShaderLoader()
{
}

