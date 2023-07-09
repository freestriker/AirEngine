#include "ShaderLoader.hpp"
#include <fstream>
#include "../Core/Manager/GraphicDeviceManager.hpp"
#include "../Graphic/Rendering/Shader.hpp"
#include "../Utility/InternedString.hpp"
#include <spirv_reflect.h>

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
struct SubShaderCreateInfo
{
	std::unordered_map<vk::ShaderStageFlagBits, std::pair<SpvReflectShaderModule, VkShaderModule>> shaderDatas;
};
struct ShaderCreateInfo
{
	std::unordered_map<std::string, SubShaderCreateInfo> subShaderCreateInfos;
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

	*isInLoading = false;
}

AirEngine::Runtime::AssetLoader::ShaderLoader::ShaderLoader()
	: AssetLoaderBase("ShaderLoader", "shader")
{

}

AirEngine::Runtime::AssetLoader::ShaderLoader::~ShaderLoader()
{
}

