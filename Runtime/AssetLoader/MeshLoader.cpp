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
	//aiProcess_PreTransformVertices |
	aiProcess_ImproveCacheLocality |
	aiProcess_GenBoundingBoxes |
	aiProcess_GenUVCoords | 0;

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
	}

	Assimp::Importer importer{};
	{
		std::filesystem::path meshPath(descriptor.meshPath);
		if (!std::filesystem::exists(meshPath)) qFatal("Mesh do not exist.");
	}
	auto&& scene = importer.ReadFile(descriptor.meshPath, postProcessSteps);
	if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode) qFatal(importer.GetErrorString());

	using VertexData = Asset::Mesh::VertexData;
	using SubMeshInfo = Asset::Mesh::SubMeshInfo;

	const auto rootNode = scene->mRootNode;
	const uint32_t meshCount = scene->mNumMeshes;

	std::vector<SubMeshInfo> subMeshInfos(meshCount, SubMeshInfo{});
	uint32_t totalVertexCount = 0;
	std::vector<std::vector<VertexData>> perMeshVertexDatas{meshCount};
	uint32_t totalIndexCount = 0;
	std::vector<std::vector<uint32_t>> perMeshIndexs{meshCount};
	for (uint32_t subMeshIndex = 0; subMeshIndex < meshCount; subMeshIndex++)
	{
		auto&& subMesh = scene->mMeshes[subMeshIndex];
		auto&& subMeshInfo = subMeshInfos[subMeshIndex];

		subMeshInfo.name = Utility::InternedString::InternedString(subMesh->mName.C_Str());

		//vertex
		{
			auto&& vertexDatas = perMeshVertexDatas[subMeshIndex];
			auto&& vertexCount = subMesh->mNumVertices;

			subMeshInfo.vertexOffset = totalVertexCount;
			subMeshInfo.vertexCount = vertexCount;
			totalVertexCount += vertexCount;
			vertexDatas.resize(vertexCount);

			for (uint32_t vertexIndex = 0; vertexIndex < vertexCount; vertexIndex++)
			{
				auto& vertexData = vertexDatas[vertexIndex];

				// positions
				{
					auto& position = vertexData.position;
					auto& vertexPosition = subMesh->mVertices[vertexIndex];

					position.x = vertexPosition.x;
					position.y = vertexPosition.y;
					position.z = vertexPosition.z;
				}
				// normals
				{
					auto& normal = vertexData.normal;
					auto& vertexNormal = subMesh->mNormals[vertexIndex];

					normal.x = vertexNormal.x;
					normal.y = vertexNormal.y;
					normal.z = vertexNormal.z;

				}
				// tangent
				{
					auto& tangent = vertexData.tangent;
					auto& vertexTangent = subMesh->mTangents[vertexIndex];

					tangent.x = vertexTangent.x;
					tangent.y = vertexTangent.y;
					tangent.z = vertexTangent.z;

				}
				// texture coords
				{
					auto& texCoords = vertexData.texCoords;
					auto& vertexTexCoords = subMesh->mTextureCoords[0][vertexIndex];

					texCoords.x = vertexTexCoords.x;
					texCoords.y = vertexTexCoords.y;
				}
			}
		}

		//index
		{
			auto&& indexDatas = perMeshIndexs[subMeshIndex];
			auto&& faceCount = subMesh->mNumFaces;
			auto&& indexCount = faceCount * 3;

			subMeshInfo.indexOffset = totalIndexCount;
			subMeshInfo.indexCount = indexCount;
			totalIndexCount += indexCount;
			indexDatas.resize(indexCount);

			for (uint32_t faceIndex = 0, indexIndex = 0; faceIndex < subMesh->mNumFaces; faceIndex++, indexIndex += 3)
			{
				auto&& face = subMesh->mFaces[faceIndex];

				indexDatas[indexIndex + 0] = subMeshInfo.vertexOffset + face.mIndices[0];
				indexDatas[indexIndex + 1] = subMeshInfo.vertexOffset + face.mIndices[1];
				indexDatas[indexIndex + 2] = subMeshInfo.vertexOffset + face.mIndices[2];
			}
		}
	}

	uint8_t perIndexByteCount = 0;
	void* stagingIndexDatas = nullptr;
	void* stagingIndexDatasPtr = nullptr;
	if (totalIndexCount <= std::numeric_limits<uint8_t>::max())
	{
		perIndexByteCount = 1;
		std::vector<uint8_t>* indexDatas = new std::vector<uint8_t>(totalIndexCount);
		stagingIndexDatas = indexDatas;
		stagingIndexDatasPtr = indexDatas->data();

		for (uint32_t meshIndex = 0; meshIndex < meshCount; meshIndex++)
		{
			auto&& subMeshInfo = subMeshInfos[meshIndex];
			auto&& meshIndexs = perMeshIndexs[meshIndex];
			auto&& indexCount = subMeshInfo.indexCount;
			auto&& indexOffset = subMeshInfo.indexOffset;
			for (uint32_t i = 0; i < indexCount; i++)
			{
				(*indexDatas)[indexOffset + i] = meshIndexs[i];
			}
		}
	}
	else if (totalIndexCount <= std::numeric_limits<uint16_t>::max())
	{
		perIndexByteCount = 2;
		std::vector<uint16_t>* indexDatas = new std::vector<uint16_t>(totalIndexCount);
		stagingIndexDatas = indexDatas;
		stagingIndexDatasPtr = indexDatas->data();

		for (uint32_t meshIndex = 0; meshIndex < meshCount; meshIndex++)
		{
			auto&& subMeshInfo = subMeshInfos[meshIndex];
			auto&& meshIndexs = perMeshIndexs[meshIndex];
			auto&& indexCount = subMeshInfo.indexCount;
			auto&& indexOffset = subMeshInfo.indexOffset;
			for (uint32_t i = 0; i < indexCount; i++)
			{
				(*indexDatas)[indexOffset + i] = meshIndexs[i];
			}
		}
	}
	else
	{
		perIndexByteCount = 4;
		std::vector<uint32_t>* indexDatas = new std::vector<uint32_t>(totalIndexCount);
		stagingIndexDatas = indexDatas;
		stagingIndexDatasPtr = indexDatas->data();

		for (uint32_t meshIndex = 0; meshIndex < meshCount; meshIndex++)
		{
			(*indexDatas).insert((*indexDatas).end(), perMeshIndexs[meshIndex].begin(), perMeshIndexs[meshIndex].end());
		}
	}

	auto&& stagingBuffer = Graphic::Instance::Buffer(
		sizeof(VertexData) * totalVertexCount + perIndexByteCount * totalIndexCount,
		VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
		VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
		VmaAllocationCreateFlagBits::VMA_ALLOCATION_CREATE_HOST_ACCESS_SEQUENTIAL_WRITE_BIT
	);
	{
		auto data = stagingBuffer.Memory()->Map();
		for (uint32_t meshIndex = 0; meshIndex < meshCount; meshIndex++)
		{
			auto&& subMeshInfo = subMeshInfos[meshIndex];
			auto&& meshVertexDatas = perMeshVertexDatas[meshIndex];

			std::memcpy(
				static_cast<VertexData*>(data) + subMeshInfo.vertexOffset,
				meshVertexDatas.data(),
				subMeshInfo.vertexCount * sizeof(VertexData)
			);
		}
		std::memcpy(
			static_cast<VertexData*>(data) + totalVertexCount,
			stagingIndexDatasPtr,
			perIndexByteCount * totalIndexCount
		);
		stagingBuffer.Memory()->Unmap();
	}

	switch (perIndexByteCount)
	{
		case 1:
		{
			std::vector<uint8_t>* indexDatas = reinterpret_cast<std::vector<uint8_t>*>(stagingIndexDatas);
			delete indexDatas;
			break;
		}
		case 2:
		{
			std::vector<uint16_t>* indexDatas = reinterpret_cast<std::vector<uint16_t>*>(stagingIndexDatas);
			delete indexDatas;
			break;
		}
		case 4:
		{
			std::vector<uint32_t>* indexDatas = reinterpret_cast<std::vector<uint32_t>*>(stagingIndexDatas);
			delete indexDatas;
			break;
		}
		default:
		{
			qFatal("Failed to delete staging data.");
			break;
		}
	}

	auto&& vertexBuffer = new Graphic::Instance::Buffer(
		sizeof(VertexData) * totalVertexCount,
		VK_BUFFER_USAGE_VERTEX_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT,
		VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT
	);

	auto&& indexBuffer = new Graphic::Instance::Buffer(
		perIndexByteCount * totalIndexCount,
		VK_BUFFER_USAGE_INDEX_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT,
		VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT
	);

	//copy to device
	{
		auto&& commandPool = Graphic::Command::CommandPool(Utility::InternedString("TransferQueue"), VK_COMMAND_POOL_CREATE_TRANSIENT_BIT);
		auto&& commandBuffer = commandPool.CreateCommandBuffer(Utility::InternedString("TransferCommandBuffer"));
		auto&& transferFence = Graphic::Command::Fence(false);

		commandBuffer.BeginRecord(VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT);
		commandBuffer.CopyBuffer(stagingBuffer, *vertexBuffer, { {0, 0, sizeof(VertexData) * totalVertexCount} });
		commandBuffer.CopyBuffer(stagingBuffer, *indexBuffer, { {sizeof(VertexData) * totalVertexCount, 0, perIndexByteCount * totalIndexCount} });
		commandBuffer.EndRecord();

		commandPool.Queue().ImmediateIndividualSubmit(
			{ {}, { &commandBuffer }, {} },
			transferFence
		);

		while (transferFence.Status() == VK_NOT_READY) Utility::ThisFiber::yield();
	}

	mesh->_vertexBuffer = vertexBuffer;
	mesh->_indexBuffer = indexBuffer;
	mesh->_subMeshInfos = std::move(subMeshInfos);
	mesh->_perIndexByteCount = perIndexByteCount;
	switch (perIndexByteCount)
	{
		case 1:
		{
			mesh->_indexType = VK_INDEX_TYPE_UINT8_EXT;	
			break;
		}
		case 2:
		{
			mesh->_indexType = VK_INDEX_TYPE_UINT16;
			break;
		}
		case 4:
		{
			mesh->_indexType = VK_INDEX_TYPE_UINT32;
			break;
		}
		default:
		{
			qFatal("Failed get index type.");
			break;
		}
	}

	*isInLoading = false;
}

AirEngine::Runtime::AssetLoader::MeshLoader::MeshLoader()
	: AssetLoaderBase("MeshLoader", "mesh")
{

}
AirEngine::Runtime::AssetLoader::MeshLoader::~MeshLoader()
{
}
