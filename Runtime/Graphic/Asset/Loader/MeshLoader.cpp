#include "MeshLoader.hpp"
#include <fstream>
#include "AirEngine/Runtime/Graphic/Manager/DeviceManager.hpp"
#include "AirEngine/Runtime/Graphic/Instance/Buffer.hpp"
#include "AirEngine/Runtime/Graphic/Command/CommandPool.hpp"
#include "AirEngine/Runtime/Graphic/Command/CommandBuffer.hpp"
#include "AirEngine/Runtime/Graphic/Command/Fence.hpp"
#include "AirEngine/Runtime/Graphic/Command/Barrier.hpp"
#include "AirEngine/Runtime/Graphic/Asset/Mesh.hpp"
#include <assimp/postprocess.h>
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <glm/vec2.hpp>
#include <glm/vec3.hpp>
#include "AirEngine/Runtime/Graphic/MeshAttributePaser/MeshAttributePaserBase.hpp"
#include "AirEngine/Runtime/Utility/ReflectableObject.hpp"
#include "AirEngine/Runtime/Utility/StringToAssimpTypeTransfer.hpp"
#include "AirEngine/Runtime/Graphic/Command/Barrier.hpp"
#include "AirEngine/Runtime/Core/Manager/TaskManager.hpp"

AirEngine::Runtime::Asset::AssetBase* AirEngine::Runtime::Graphic::Asset::Loader::MeshLoader::OnLoadAsset(const std::string& path, std::shared_future<void>& loadOperationFuture, bool& isInLoading)
{
	auto&& Mesh = new Asset::Mesh();
	bool* isLoadingPtr = &isInLoading;

	loadOperationFuture = std::move(
		std::shared_future<void>(
			std::move(
				Core::Manager::TaskManager::Executor().async(
					[Mesh, path, isLoadingPtr]()->void
					{
						PopulateMesh(Mesh, path, isLoadingPtr);
					}
				)
			)
		)
	);

	return Mesh;
}

void AirEngine::Runtime::Graphic::Asset::Loader::MeshLoader::OnUnloadAsset(AirEngine::Runtime::Asset::AssetBase* asset)
{
	delete static_cast<AirEngine::Runtime::Graphic::Asset::Mesh*>(asset);
}

void AirEngine::Runtime::Graphic::Asset::Loader::MeshLoader::PopulateMesh(AirEngine::Runtime::Graphic::Asset::Mesh* mesh, const std::string path, bool* isInLoading)
{
	// Load descriptor
	Descriptor descriptor{};
	{
		std::ifstream descriptorFile(path);
		if (!descriptorFile.is_open()) qFatal("Failed to open file.");
		nlohmann::json jsonFile = nlohmann::json::parse((std::istreambuf_iterator<char>(descriptorFile)), std::istreambuf_iterator<char>());
		descriptor = jsonFile.get<Descriptor>();
		descriptorFile.close();
	}

	std::unique_ptr<Graphic::MeshAttributePaser::MeshAttributePaserBase> meshAttributePaser;
	/// create mesh attribute paser
	{
		std::string ptrTypeName(descriptor.meshAttributePaser + "*");
		int typeId = Utility::MetaType::type(ptrTypeName.c_str());
		const Utility::MetaObject* metaObj = Utility::MetaType::metaObjectForType(typeId);
		if (metaObj == nullptr)
		{
			qFatal(std::string("Do not have mesh attribute paser type named: " + ptrTypeName + ".").c_str());
		}

		Utility::ReflectableObject* obj = metaObj->newInstance();
		if (obj == nullptr)
		{
			qFatal(std::string("Can not create mesh attribute paser variant named: " + ptrTypeName + ".").c_str());
		}

		auto&& ptr = qobject_cast<Graphic::MeshAttributePaser::MeshAttributePaserBase*>(obj);
		if (ptr == nullptr)
		{
			qFatal(std::string(ptrTypeName + " can not cast to mesh attribute paser base.").c_str());
		}

		meshAttributePaser.reset(ptr);
	}
	//Parse descriptor data
	uint32_t postProcessSteps = Utility::StringToAssimpTypeTransfer::DEFAULT_POST_PROCESS_STEPS;
	postProcessSteps |= Utility::StringToAssimpTypeTransfer::PostProcessStepsStringsToPostProcessSteps(descriptor.postProcessSteps);
	meshAttributePaser->OnEditPostProcessSteps(postProcessSteps);
	postProcessSteps = Utility::StringToAssimpTypeTransfer::RemoveInvalidPostProcessSteps(postProcessSteps);

	Assimp::Importer importer{};
	{
		std::filesystem::path meshPath(descriptor.meshPath);
		if (!std::filesystem::exists(meshPath)) qFatal("Mesh do not exist.");
	}
	auto&& scene = importer.ReadFile(descriptor.meshPath, postProcessSteps);
	if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode) qFatal(importer.GetErrorString());

	//using VertexData = Asset::Mesh::VertexData;
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

	const auto PER_VERTEX_DATA_BYTE_SIZE = meshAttributePaser->OnGetPerVertexByteSize();
	const auto PER_INDEX_DATA_BYTE_SIZE = Asset::Mesh::IndexTypeToByteCount(meshInfo.indexType);
	const auto VERTEX_DATA_BYTE_SIZE = meshInfo.vertexCount * PER_VERTEX_DATA_BYTE_SIZE;
	const auto INDEX_DATA_BYTE_SIZE = meshInfo.indexCount * PER_INDEX_DATA_BYTE_SIZE;
	const auto DATA_BYTE_SIZE = VERTEX_DATA_BYTE_SIZE + INDEX_DATA_BYTE_SIZE;
	const auto VERTEX_DATA_BYTE_OFFSET = 0;
	const auto INDEX_DATA_BYTE_OFFSET = VERTEX_DATA_BYTE_OFFSET + VERTEX_DATA_BYTE_SIZE;

	meshInfo.vertexByteSize = PER_VERTEX_DATA_BYTE_SIZE;

	/// populate mesh vertex attribute info map
	{
		meshAttributePaser->OnPopulateMeshVertexAttributeInfoMap(meshInfo.meshVertexAttributeInfoMap);
	}

	std::vector<uint8_t> dataBytes(DATA_BYTE_SIZE);

	/// populate vertex data
	{
		meshAttributePaser->OnPopulateVertexData(dataBytes.data(), VERTEX_DATA_BYTE_SIZE, meshInfo, *scene);
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
		vk::MemoryPropertyFlagBits::eHostVisible | vk::MemoryPropertyFlagBits::eHostCached,
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
		stagingBuffer.Memory()->Flush();
	}

	// copy to device
	{
		//Graphic::Command::Barrier barrier{};
		//barrier.AddBufferMemoryBarrier(
		//	*vertexBuffer,
		//	vk::PipelineStageFlagBits2::eHost,
		//	vk::AccessFlagBits2::eHostWrite,
		//	vk::PipelineStageFlagBits2::eTransfer,
		//	vk::AccessFlagBits2::eTransferWrite
		//);
		//barrier.AddBufferMemoryBarrier(
		//	*indexBuffer,
		//	vk::PipelineStageFlagBits2::eHost,
		//	vk::AccessFlagBits2::eHostWrite,
		//	vk::PipelineStageFlagBits2::eTransfer,
		//	vk::AccessFlagBits2::eTransferWrite
		//);
		
		auto&& commandPool = Graphic::Command::CommandPool(Utility::InternedString("TransferQueue"), vk::CommandPoolCreateFlagBits::eTransient);
		auto&& commandBuffer = commandPool.CreateCommandBuffer(Utility::InternedString("TransferCommandBuffer"));
		auto&& transferFence = Graphic::Command::Fence(false);

		commandBuffer.BeginRecord(vk::CommandBufferUsageFlagBits::eOneTimeSubmit);
		//commandBuffer.AddPipelineBarrier(barrier);
		commandBuffer.CopyBuffer(stagingBuffer, *vertexBuffer, { {VERTEX_DATA_BYTE_OFFSET, 0, VERTEX_DATA_BYTE_SIZE} });
		commandBuffer.CopyBuffer(stagingBuffer, *indexBuffer, { {INDEX_DATA_BYTE_OFFSET, 0, INDEX_DATA_BYTE_SIZE} });
		commandBuffer.EndRecord();

		commandPool.Queue().ImmediateIndividualSubmit(
			{ {}, { &commandBuffer }, {} },
			transferFence
		);

		while (transferFence.Status() == vk::Result::eNotReady) std::this_thread::yield();
	}

	mesh->_vertexBuffer = vertexBuffer;
	mesh->_indexBuffer = indexBuffer;
	mesh->_meshInfo = std::move(meshInfo);

	*isInLoading = false;
}

AirEngine::Runtime::Graphic::Asset::Loader::MeshLoader::MeshLoader()
	: LoaderBase("MeshLoader", "mesh")
{

}
AirEngine::Runtime::Graphic::Asset::Loader::MeshLoader::~MeshLoader()
{
}
