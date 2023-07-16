#include "ShaderLoader.hpp"
#include <fstream>
#include "../Core/Manager/GraphicDeviceManager.hpp"
#include "../Graphic/Rendering/Shader.hpp"
#include "../Utility/InternedString.hpp"
#include <spirv_reflect.h>
#include <unordered_map>
#include "../Graphic/Instance/RenderPassBase.hpp"
#include "../Graphic/Manager/RenderPassManager.hpp"
#include "../Core/Manager/GraphicDeviceManager.hpp"
#include "../Graphic/Manager/ShaderManager.hpp"
#include <vulkan/vulkan.hpp>
#include <vulkan/vulkan_core.h>

AirEngine::Runtime::Asset::AssetBase* AirEngine::Runtime::AssetLoader::ShaderLoader::OnLoadAsset(const std::string& path, Utility::Fiber::shared_future<void>& loadOperationFuture, bool& isInLoading)
{
	auto&& Mesh = NEW_COLLECTABLE_PURE_OBJECT Graphic::Rendering::Shader();
	Utility::Fiber::packaged_task<void(AirEngine::Runtime::Graphic::Rendering::Shader*, const std::string, bool*)> packagedTask(PopulateShader);
	loadOperationFuture = std::move(Utility::Fiber::shared_future<void>(std::move(packagedTask.get_future())));
	Utility::Fiber::fiber(std::move(packagedTask), Mesh, path, &isInLoading).detach();
	return Mesh;
}

void AirEngine::Runtime::AssetLoader::ShaderLoader::OnUnloadAsset(AirEngine::Runtime::Asset::AssetBase* asset)
{
	delete static_cast<AirEngine::Runtime::Graphic::Rendering::Shader*>(asset);
}

struct SubShaderDescriptor
{
	std::string subPass{};

	std::vector<std::string> spvShaderPaths{};

	std::string cullMode;

	bool depthTestEnable{};
	bool depthWriteEnable{};
	std::string depthCompareOp{};

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
		SubShaderDescriptor,

		subPass,

		spvShaderPaths,

		cullMode,

		depthTestEnable,
		depthWriteEnable,
		depthCompareOp,

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
struct VertexInputInfo
{
	AirEngine::Runtime::Utility::InternedString name;
	vk::Format format;
	uint32_t location;
};
struct SubShaderCreateInfo
{
	std::unordered_map<vk::ShaderStageFlagBits, std::pair<SpvReflectShaderModule, VkShaderModule>> shaderDatas;
	std::vector<vk::PipelineShaderStageCreateInfo> pipelineShaderStageCreateInfos;
	std::unordered_map< AirEngine::Runtime::Utility::InternedString, VertexInputInfo> vertexInputInfos;
};
struct ShaderCreateInfo
{
	std::unordered_map<std::string, SubShaderCreateInfo> subShaderCreateInfos;
	AirEngine::Runtime::Graphic::Instance::RenderPassBase* renderPass;
};

void AllocateDataSpace(const ShaderDescriptor& shaderDescriptor, ShaderCreateInfo& shaderCreateInfo)
{
	for (const auto& subShaderDescriptor : shaderDescriptor.subShaders)
	{
		shaderCreateInfo.subShaderCreateInfos[subShaderDescriptor.subPass] = {};
	}
}

void LoadSpirvData(const ShaderDescriptor& shaderDescriptor, ShaderCreateInfo& shaderCreateInfo)
{
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
			vk::Device device(AirEngine::Runtime::Core::Manager::GraphicDeviceManager::VkDevice());
			shaderData.second = device.createShaderModule(shaderModuleCreateInfo);

			auto stage = vk::ShaderStageFlagBits(shaderData.first.shader_stage);
			if(subShaderCreateInfo.shaderDatas.contains(stage)) qFatal("Failed to load the same shader stage.");
			
			subShaderCreateInfo.shaderDatas[stage] = std::move(shaderData);
		}

		subShaderCreateInfo.pipelineShaderStageCreateInfos.reserve(subShaderCreateInfo.shaderDatas.size());
		for (const auto& shaderDataPair : subShaderCreateInfo.shaderDatas)
		{
			subShaderCreateInfo.pipelineShaderStageCreateInfos.emplace_back(vk::PipelineShaderStageCreateFlags(), shaderDataPair.first, shaderDataPair.second.second, shaderDataPair.second.first.entry_point_name);
		}
	}
}

void LoadVertexInputData(const ShaderDescriptor& shaderDescriptor, ShaderCreateInfo& shaderCreateInfo)
{
	for (const auto& subShaderDescriptor : shaderDescriptor.subShaders)
	{
		auto& subShaderCreateInfo = shaderCreateInfo.subShaderCreateInfos.at(subShaderDescriptor.subPass);
		const auto& vertexShaderReflectData = subShaderCreateInfo.shaderDatas.at(vk::ShaderStageFlagBits::eVertex).first;

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

			VertexInputInfo vertexInputInfo{};
			vertexInputInfo.name = name;
			vertexInputInfo.location = inputVar.location;
			vertexInputInfo.format = vk::Format(inputVar.format);

			subShaderCreateInfo.vertexInputInfos.emplace(name, std::move(vertexInputInfo));
		}
	}
}

void LoadRenderPassData(const ShaderDescriptor& shaderDescriptor, ShaderCreateInfo& shaderCreateInfo)
{
	shaderCreateInfo.renderPass = AirEngine::Runtime::Graphic::Manager::RenderPassManager::LoadRenderPass(shaderDescriptor.renderPass);
}

void CheckShaderStageInOutData(const ShaderDescriptor& shaderDescriptor, ShaderCreateInfo& shaderCreateInfo)
{
	constexpr auto GRAPHIC_SHADER_STAGES{ 
		std::to_array<vk::ShaderStageFlagBits>(
		{ 
			vk::ShaderStageFlagBits::eVertex,
			vk::ShaderStageFlagBits::eTessellationControl,
			vk::ShaderStageFlagBits::eTessellationEvaluation,
			vk::ShaderStageFlagBits::eGeometry,
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
	}
}

void ParseShaderInfo(AirEngine::Runtime::Graphic::Rendering::Shader::ShaderInfo& shaderInfo, const ShaderDescriptor& shaderDescriptor, ShaderCreateInfo& shaderCreateInfo)
{
	using namespace AirEngine::Runtime::Graphic::Rendering;

	std::unordered_map<AirEngine::Runtime::Utility::InternedString, std::tuple<uint16_t, uint8_t>> descriptorInfoNameToStartIndexAndCountMap{};
	std::vector<uint16_t> sameNameCompactDescriptorIndexs{};

	for (const auto& subShaderDescriptor : shaderDescriptor.subShaders)
	{
		const auto& subShaderCreateInfo = shaderCreateInfo.subShaderCreateInfos.at(subShaderDescriptor.subPass);

		auto subPassName = AirEngine::Runtime::Utility::InternedString(subShaderDescriptor.subPass);

		std::map<uint8_t, std::map<uint8_t, vk::DescriptorSetLayoutBinding>> setToBindingToDescriptorBindingMap{};
		std::map<uint8_t, std::pair<Shader::DescriptorSetInfo, std::map<uint8_t, Shader::DescriptorInfo>>> setToDescriptorSetInfoAndBindingToDescriptorInfoMap{};

		//parse direct data
		for (const auto& shaderReflectDataPair : subShaderCreateInfo.shaderDatas)
		{
			const auto& shaderReflectData = shaderReflectDataPair.second.first;

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

				auto&& descriptorSetInfoAndBindingToDescriptorInfoMapPair = setToDescriptorSetInfoAndBindingToDescriptorInfoMap[reflectDescriptorSet.set];
				auto&& descriptorSetInfo = descriptorSetInfoAndBindingToDescriptorInfoMapPair.first;
				descriptorSetInfo.set = reflectDescriptorSet.set;
				auto&& bindingToDescriptorInfoMap = descriptorSetInfoAndBindingToDescriptorInfoMapPair.second;

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
							descriptorInfo.imageInfo = std::make_unique<Shader::DescriptorInfo::ImageInfo>
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

		//create layout and size
		for (const auto& bindingToDescriptorBindingMapPair : setToBindingToDescriptorBindingMap)
		{
			const auto& set = bindingToDescriptorBindingMapPair.first;
			const auto& bindingToDescriptorBindingMap = bindingToDescriptorBindingMapPair.second;

			std::vector<vk::DescriptorSetLayoutBinding> vkDescriptorSetLayoutBindings{};
			for (const auto& descriptorPair : bindingToDescriptorBindingMap)
			{
				const auto& descriptor = descriptorPair.second;

				vkDescriptorSetLayoutBindings.emplace_back(descriptor);
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

			auto&& layout = vk::Device(AirEngine::Runtime::Core::Manager::GraphicDeviceManager::VkDevice()).createDescriptorSetLayout(descriptorSetLayoutCreateInfo);

			for(auto& bindingToDescriptorInfoMapPair: setToDescriptorSetInfoAndBindingToDescriptorInfoMap.at(set).second)
			{
				const auto binding = bindingToDescriptorInfoMapPair.first;
				auto& descriptorInfo = bindingToDescriptorInfoMapPair.second;
				descriptorInfo.singleDescriptorByteSize = AirEngine::Runtime::Graphic::Manager::ShaderManager::DescriptorSize(descriptorInfo.type);
				descriptorInfo.solidByteSizeInDescriptorSet = descriptorInfo.singleDescriptorByteSize * descriptorInfo.descriptorCount;
				descriptorInfo.startByteOffsetInDescriptorSet = uint16_t(vk::Device(AirEngine::Runtime::Core::Manager::GraphicDeviceManager::VkDevice()).getDescriptorSetLayoutBindingOffsetEXT(layout, uint32_t(binding)));
			}

			auto&& descriptorSetInfo = setToDescriptorSetInfoAndBindingToDescriptorInfoMap.at(set).first;
			descriptorSetInfo.layout = layout;
			descriptorSetInfo.isDynamicByteSize = isDynamic;
			descriptorSetInfo.solidByteSize = 
				isDynamic ? 
				setToDescriptorSetInfoAndBindingToDescriptorInfoMap.at(set).second.rbegin()->second.startByteOffsetInDescriptorSet: 
				uint16_t(vk::Device(AirEngine::Runtime::Core::Manager::GraphicDeviceManager::VkDevice()).getDescriptorSetLayoutSizeEXT(layout));
		}

		//compact object
		auto& subShaderInfo = shaderInfo.subShaderInfoMap[subPassName];
		subShaderInfo.subPass = subPassName;
		for (auto& setToDescriptorSetInfoAndBindingToDescriptorInfoMapPair : setToDescriptorSetInfoAndBindingToDescriptorInfoMap)
		{
			const auto& set = setToDescriptorSetInfoAndBindingToDescriptorInfoMapPair.first;
			auto&& bindingToDescriptorInfoMap = setToDescriptorSetInfoAndBindingToDescriptorInfoMapPair.second.second;
			auto&& descriptorSetInfo = setToDescriptorSetInfoAndBindingToDescriptorInfoMapPair.second.first;

			descriptorSetInfo.index = subShaderInfo.descriptorSetInfos.size();

			subShaderInfo.descriptorSetInfos.emplace_back(descriptorSetInfo);

			for (auto& bindingToDescriptorInfoMapPair : bindingToDescriptorInfoMap)
			{
				const auto& binding = bindingToDescriptorInfoMapPair.first;
				auto& descriptorInfo = bindingToDescriptorInfoMapPair.second;

				descriptorInfo.index = subShaderInfo.descriptorInfos.size();
				descriptorInfo.descriptorSetInfo = reinterpret_cast<Shader::DescriptorSetInfo*>(descriptorSetInfo.index);

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
			descriptorInfo.descriptorSetInfo = reinterpret_cast<const Shader::DescriptorSetInfo*>(descriptorSetInfo);
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
	

	AllocateDataSpace(shaderDescriptor, shaderCreateInfo);
	LoadSpirvData(shaderDescriptor, shaderCreateInfo);
	LoadVertexInputData(shaderDescriptor, shaderCreateInfo);
	LoadRenderPassData(shaderDescriptor, shaderCreateInfo);
	CheckShaderStageInOutData(shaderDescriptor, shaderCreateInfo);
	ParseShaderInfo(shader->_shaderInfo, shaderDescriptor, shaderCreateInfo);
	
	*isInLoading = false;
}

AirEngine::Runtime::AssetLoader::ShaderLoader::ShaderLoader()
	: AssetLoaderBase("ShaderLoader", "shader")
{

}

AirEngine::Runtime::AssetLoader::ShaderLoader::~ShaderLoader()
{
}

