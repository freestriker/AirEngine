#include "MeshLoader.hpp"
#include <fstream>
#include "../Core/Manager/GraphicDeviceManager.hpp"
#include "../Graphic/Instance/Buffer.hpp"
#include "../Graphic/Command/CommandPool.hpp"
#include "../Graphic/Command/CommandBuffer.hpp"
#include "../Graphic/Command/Fence.hpp"
#include "../Graphic/Command/Barrier.hpp"
#include "../Asset/Mesh.hpp"
#include "../Utility/Fiber.hpp"
#include <assimp/postprocess.h>
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <glm/vec2.hpp>
#include <glm/vec3.hpp>

const static std::unordered_map<std::string, uint32_t> AI_POST_PROCESS_STEPS_MAP
{
	{ "aiProcess_CalcTangentSpace", aiProcess_CalcTangentSpace },
	{ "aiProcess_JoinIdenticalVertices", aiProcess_JoinIdenticalVertices },
	{ "aiProcess_MakeLeftHanded", aiProcess_MakeLeftHanded },
	{ "aiProcess_Triangulate", aiProcess_Triangulate },
	{ "aiProcess_RemoveComponent", aiProcess_RemoveComponent },
	{ "aiProcess_GenNormals", aiProcess_GenNormals },
	{ "aiProcess_GenSmoothNormals", aiProcess_GenSmoothNormals },
	{ "aiProcess_SplitLargeMeshes", aiProcess_SplitLargeMeshes },
	{ "aiProcess_PreTransformVertices", aiProcess_PreTransformVertices },
	{ "aiProcess_LimitBoneWeights", aiProcess_LimitBoneWeights },
	{ "aiProcess_ValidateDataStructure", aiProcess_ValidateDataStructure },
	{ "aiProcess_ImproveCacheLocality", aiProcess_ImproveCacheLocality },
	{ "aiProcess_RemoveRedundantMaterials", aiProcess_RemoveRedundantMaterials },
	{ "aiProcess_FixInfacingNormals", aiProcess_FixInfacingNormals },
	{ "aiProcess_PopulateArmatureData", aiProcess_PopulateArmatureData },
	{ "aiProcess_SortByPType", aiProcess_SortByPType },
	{ "aiProcess_FindDegenerates", aiProcess_FindDegenerates },
	{ "aiProcess_FindInvalidData", aiProcess_FindInvalidData },
	{ "aiProcess_GenUVCoords", aiProcess_GenUVCoords },
	{ "aiProcess_TransformUVCoords", aiProcess_TransformUVCoords },
	{ "aiProcess_FindInstances", aiProcess_FindInstances },
	{ "aiProcess_OptimizeMeshes", aiProcess_OptimizeMeshes },
	{ "aiProcess_OptimizeGraph", aiProcess_OptimizeGraph },
	{ "aiProcess_FlipUVs", aiProcess_FlipUVs },
	{ "aiProcess_FlipWindingOrder", aiProcess_FlipWindingOrder },
	{ "aiProcess_SplitByBoneCount", aiProcess_SplitByBoneCount },
	{ "aiProcess_Debone", aiProcess_Debone },
	{ "aiProcess_GlobalScale", aiProcess_GlobalScale },
	{ "aiProcess_EmbedTextures", aiProcess_EmbedTextures },
	{ "aiProcess_ForceGenNormals", aiProcess_ForceGenNormals },
	{ "aiProcess_DropNormals", aiProcess_DropNormals },
	{ "aiProcess_GenBoundingBoxes", aiProcess_GenBoundingBoxes },
	{ "aiProcess_ConvertToLeftHanded", aiProcess_ConvertToLeftHanded },
	{ "aiProcessPreset_TargetRealtime_Fast", aiProcessPreset_TargetRealtime_Fast },
	{ "aiProcessPreset_TargetRealtime_Quality", aiProcessPreset_TargetRealtime_Quality },
	{ "aiProcessPreset_TargetRealtime_MaxQuality", aiProcessPreset_TargetRealtime_MaxQuality }
};

constexpr uint32_t DEFAULT_POST_PROCESS_STEPS =
	aiProcess_JoinIdenticalVertices |
	aiProcess_CalcTangentSpace |
	aiProcess_Triangulate |
	aiProcess_GenNormals |
	aiProcess_ImproveCacheLocality |
	aiProcess_GenBoundingBoxes |
	aiProcess_GenUVCoords | 0;
constexpr uint32_t INVALID_POST_PROCESS_STEPS =
	~(aiProcess_PreTransformVertices);

AirEngine::Runtime::Asset::AssetBase* AirEngine::Runtime::AssetLoader::MeshLoader::OnLoadAsset(const std::string& path, Utility::Fiber::shared_future<void>& loadOperationFuture, bool& isInLoading)
{
	auto&& Mesh = NEW_COLLECTABLE_PURE_OBJECT Asset::Mesh();
	Utility::Fiber::packaged_task<void(AirEngine::Runtime::Asset::Mesh*, const std::string, bool*)> packagedTask(PopulateMesh);
	loadOperationFuture = std::move(Utility::Fiber::shared_future<void>(std::move(packagedTask.get_future())));
	Utility::Fiber::fiber(std::move(packagedTask), Mesh, path, &isInLoading).detach();
	return Mesh;
}

void AirEngine::Runtime::AssetLoader::MeshLoader::OnUnloadAsset(AirEngine::Runtime::Asset::AssetBase* asset)
{
	delete static_cast<AirEngine::Runtime::Asset::Mesh*>(asset);
}

void AirEngine::Runtime::AssetLoader::MeshLoader::PopulateMesh(AirEngine::Runtime::Asset::Mesh* mesh, const std::string path, bool* isInLoading)
{
	//Load descriptor
	Descriptor descriptor{};
	{
		std::ifstream descriptorFile(path);
		if (!descriptorFile.is_open()) qFatal("Failed to open file.");
		nlohmann::json jsonFile = nlohmann::json::parse((std::istreambuf_iterator<char>(descriptorFile)), std::istreambuf_iterator<char>());
		descriptor = jsonFile.get<Descriptor>();
		descriptorFile.close();
	}

	//Parse descriptor data
	uint32_t postProcessSteps = DEFAULT_POST_PROCESS_STEPS;
	{
		for (const auto& postProcessStep : descriptor.postProcessSteps)
		{
			postProcessSteps |= AI_POST_PROCESS_STEPS_MAP.at(postProcessStep);
		}
		postProcessSteps &= INVALID_POST_PROCESS_STEPS;
	}

	Assimp::Importer importer{};
	{
		std::filesystem::path meshPath(descriptor.meshPath);
		if (!std::filesystem::exists(meshPath)) qFatal("Mesh do not exist.");
	}
	auto&& scene = importer.ReadFile(descriptor.meshPath, postProcessSteps);
	if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode) qFatal(importer.GetErrorString());

	using VertexData = Asset::Mesh::VertexData;
	using MeshInfo = Asset::Mesh::MeshInfo;
	using SubMeshInfo = Asset::Mesh::SubMeshInfo;

	MeshInfo meshInfo{};

	/// get sub mesh count
	{
		meshInfo.meshCount = scene->mNumMeshes;
		meshInfo.subMeshInfo = std::vector<SubMeshInfo>(meshInfo.meshCount);
	}

	/// get vertex index offset and count
	{
		for (uint32_t subMeshIndex = 0; subMeshIndex < meshInfo.meshCount; subMeshIndex++)
		{
			const auto& subMesh = scene->mMeshes[subMeshIndex];
			auto& subMeshInfo = meshInfo.subMeshInfo[subMeshIndex];

			subMeshInfo.name = Utility::InternedString::InternedString(subMesh->mName.C_Str());
			
			subMeshInfo.vertexOffset = meshInfo.vertexCount;
			subMeshInfo.vertexCount = subMesh->mNumVertices;
			meshInfo.vertexCount += subMeshInfo.vertexCount;

			subMeshInfo.indexOffset = meshInfo.indexCount;
			subMeshInfo.indexCount = subMesh->mNumFaces * 3;
			meshInfo.indexCount += subMeshInfo.indexCount;
		}
	}

	/// get index type
	{
		if (meshInfo.indexCount <= uint32_t(std::numeric_limits<uint8_t>::max()))
		{
			meshInfo.indexType = vk::IndexType::eUint8EXT;
		}
		else if (meshInfo.indexCount <= uint32_t(std::numeric_limits<uint16_t>::max()))
		{
			meshInfo.indexType = vk::IndexType::eUint16;
		}
		else
		{
			meshInfo.indexType = vk::IndexType::eUint32;
		}
	}

	const auto PER_VERTEX_DATA_BYTE_SIZE = sizeof(VertexData);
	const auto PER_INDEX_DATA_BYTE_SIZE = Asset::Mesh::IndexTypeToByteCount(meshInfo.indexType);
	const auto VERTEX_DATA_BYTE_SIZE = meshInfo.vertexCount * PER_VERTEX_DATA_BYTE_SIZE;
	const auto INDEX_DATA_BYTE_SIZE = meshInfo.indexCount * PER_INDEX_DATA_BYTE_SIZE;
	const auto DATA_BYTE_SIZE = VERTEX_DATA_BYTE_SIZE + INDEX_DATA_BYTE_SIZE;
	const auto VERTEX_DATA_BYTE_OFFSET = 0;
	const auto INDEX_DATA_BYTE_OFFSET = VERTEX_DATA_BYTE_OFFSET + VERTEX_DATA_BYTE_SIZE;

	std::vector<uint8_t> dataBytes(DATA_BYTE_SIZE);

	/// populate vertex data
	{
		using TYPE = VertexData;
		TYPE* vertexDataPtr = reinterpret_cast<TYPE*>(dataBytes.data() + VERTEX_DATA_BYTE_OFFSET);
		for (uint32_t subMeshIndex = 0; subMeshIndex < meshInfo.meshCount; subMeshIndex++)
		{
			const auto& subMesh = scene->mMeshes[subMeshIndex];
			const auto& subMeshInfo = meshInfo.subMeshInfo[subMeshIndex];

			TYPE* subMeshVertexDataPtr = vertexDataPtr + subMeshInfo.vertexOffset;

			for (uint32_t vertexIndex = 0; vertexIndex < subMeshInfo.vertexCount; vertexIndex++)
			{
				TYPE* vertexDataPtr = subMeshVertexDataPtr + vertexIndex;

				// positions
				{
					auto& position = vertexDataPtr->position;
					auto& vertexPosition = subMesh->mVertices[vertexIndex];

					position.x = vertexPosition.x;
					position.y = vertexPosition.y;
					position.z = vertexPosition.z;
				}
				// normals
				{
					auto& normal = vertexDataPtr->normal;
					auto& vertexNormal = subMesh->mNormals[vertexIndex];

					normal.x = vertexNormal.x;
					normal.y = vertexNormal.y;
					normal.z = vertexNormal.z;

				}
				// tangent
				{
					auto& tangent = vertexDataPtr->tangent;
					auto& vertexTangent = subMesh->mTangents[vertexIndex];

					tangent.x = vertexTangent.x;
					tangent.y = vertexTangent.y;
					tangent.z = vertexTangent.z;

				}
				// texture coords
				{
					auto& texCoords = vertexDataPtr->texCoords;
					auto& vertexTexCoords = subMesh->mTextureCoords[0][vertexIndex];

					texCoords.x = vertexTexCoords.x;
					texCoords.y = vertexTexCoords.y;
				}
			}
		}
	}

#define POPULATE_INDEX_DATA(TYPE)\
{\
	TYPE* indexDataPtr = reinterpret_cast<TYPE*>(dataBytes.data() + INDEX_DATA_BYTE_OFFSET);\
	for (uint32_t subMeshIndex = 0; subMeshIndex < meshInfo.meshCount; subMeshIndex++)\
	{\
		const auto& subMesh = scene->mMeshes[subMeshIndex];\
		const auto& subMeshInfo = meshInfo.subMeshInfo[subMeshIndex];\
\
		TYPE* subMeshIndexDataPtr = indexDataPtr + subMeshInfo.indexOffset;\
\
		for (uint32_t faceIndex = 0; faceIndex < subMesh->mNumFaces; faceIndex++)\
		{\
			const auto& face = subMesh->mFaces[faceIndex];\
\
			TYPE* faseIndexDataPtr = subMeshIndexDataPtr + faceIndex * 3;\
\
			faseIndexDataPtr[0] = subMeshInfo.vertexOffset + face.mIndices[0];\
			faseIndexDataPtr[1] = subMeshInfo.vertexOffset + face.mIndices[1];\
			faseIndexDataPtr[2] = subMeshInfo.vertexOffset + face.mIndices[2];\
		}\
	}\
}\

	/// populate index data
	{
		switch (meshInfo.indexType)
		{
			case vk::IndexType::eUint8EXT:
			{
				POPULATE_INDEX_DATA(uint8_t);
				break;
			}
			case vk::IndexType::eUint16:
			{
				POPULATE_INDEX_DATA(uint16_t);
				break;
			}
			case vk::IndexType::eUint32:
			{
				POPULATE_INDEX_DATA(uint32_t);
				break;
			}
		}
	}

	auto&& vertexBuffer = new Graphic::Instance::Buffer(
		VERTEX_DATA_BYTE_SIZE,
		vk::BufferUsageFlagBits::eVertexBuffer | vk::BufferUsageFlagBits::eTransferDst,
		vk::MemoryPropertyFlagBits::eDeviceLocal
	);

	auto&& indexBuffer = new Graphic::Instance::Buffer(
		INDEX_DATA_BYTE_SIZE,
		vk::BufferUsageFlagBits::eIndexBuffer | vk::BufferUsageFlagBits::eTransferDst,
		vk::MemoryPropertyFlagBits::eDeviceLocal
	);

	auto&& stagingBuffer = Graphic::Instance::Buffer(
		DATA_BYTE_SIZE,
		vk::BufferUsageFlagBits::eTransferSrc,
		vk::MemoryPropertyFlagBits::eHostVisible | vk::MemoryPropertyFlagBits::eHostCoherent,
		VmaAllocationCreateFlagBits::VMA_ALLOCATION_CREATE_HOST_ACCESS_SEQUENTIAL_WRITE_BIT
	);

	/// copy to gpu
	{
		std::memcpy(
			stagingBuffer.Memory()->Map(),
			dataBytes.data(),
			DATA_BYTE_SIZE
		);
		stagingBuffer.Memory()->Unmap();
	}

	// copy to device
	{
		auto&& commandPool = Graphic::Command::CommandPool(Utility::InternedString("TransferQueue"), vk::CommandPoolCreateFlagBits::eTransient);
		auto&& commandBuffer = commandPool.CreateCommandBuffer(Utility::InternedString("TransferCommandBuffer"));
		auto&& transferFence = Graphic::Command::Fence(false);

		commandBuffer.BeginRecord(vk::CommandBufferUsageFlagBits::eOneTimeSubmit);
		commandBuffer.CopyBuffer(stagingBuffer, *vertexBuffer, { {VERTEX_DATA_BYTE_OFFSET, 0, VERTEX_DATA_BYTE_SIZE} });
		commandBuffer.CopyBuffer(stagingBuffer, *indexBuffer, { {INDEX_DATA_BYTE_OFFSET, 0, INDEX_DATA_BYTE_SIZE} });
		commandBuffer.EndRecord();

		commandPool.Queue().ImmediateIndividualSubmit(
			{ {}, { &commandBuffer }, {} },
			transferFence
		);

		while (transferFence.Status() == vk::Result::eNotReady) Utility::ThisFiber::yield();
	}

	mesh->_vertexBuffer = vertexBuffer;
	mesh->_indexBuffer = indexBuffer;
	mesh->_meshInfo = std::move(meshInfo);

	*isInLoading = false;
}

AirEngine::Runtime::AssetLoader::MeshLoader::MeshLoader()
	: AssetLoaderBase("MeshLoader", "mesh")
{

}
AirEngine::Runtime::AssetLoader::MeshLoader::~MeshLoader()
{
}
