#include "ShaderLoader.hpp"
#include <fstream>
#include "../Core/Manager/GraphicDeviceManager.hpp"
#include "../Graphic/Rendering/Shader.hpp"

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

	*isInLoading = false;
}

AirEngine::Runtime::AssetLoader::ShaderLoader::ShaderLoader()
	: AssetLoaderBase("ShaderLoader", "shader")
{

}

AirEngine::Runtime::AssetLoader::ShaderLoader::~ShaderLoader()
{
}

