#include "ShaderLoader.hpp"
#include <fstream>
#include "../Core/Manager/GraphicDeviceManager.hpp"
#include "../Graphic/Rendering/Shader.hpp"
#include "../Utility/InternedString.hpp"
#include <spirv_reflect.h>
#include <unordered_map>
#include "../Graphic/Instance/RenderPassBase.hpp"
#include "../Graphic/Manager/RenderPassManager.hpp"

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
	
	*isInLoading = false;
}

AirEngine::Runtime::AssetLoader::ShaderLoader::ShaderLoader()
	: AssetLoaderBase("ShaderLoader", "shader")
{

}

AirEngine::Runtime::AssetLoader::ShaderLoader::~ShaderLoader()
{
}

