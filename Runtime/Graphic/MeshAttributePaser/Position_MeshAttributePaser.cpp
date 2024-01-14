#include "Position_MeshAttributePaser.hpp"
#include <assimp/postprocess.h>

REGISTRATION
{
	DECLARE_TYPE(AirEngine::Runtime::Graphic::MeshAttributePaser::Position_MeshAttributePaser*)
}

struct VertexData
{
	glm::vec3 vertexPosition;
};

void AirEngine::Runtime::Graphic::MeshAttributePaser::Position_MeshAttributePaser::OnEditPostProcessSteps(uint32_t& postProcessSteps) const
{
}

uint32_t AirEngine::Runtime::Graphic::MeshAttributePaser::Position_MeshAttributePaser::OnGetPerVertexByteSize() const
{
	return sizeof(VertexData);
}

void AirEngine::Runtime::Graphic::MeshAttributePaser::Position_MeshAttributePaser::OnPopulateMeshVertexAttributeInfoMap(std::map<Utility::InternedString, Asset::Mesh::MeshVertexAttributeInfo>& meshVertexAttributeInfoMap) const
{
	auto&& attributeName = Utility::InternedString("vertexPosition");
	meshVertexAttributeInfoMap[attributeName] = Asset::Mesh::MeshVertexAttributeInfo(attributeName, offsetof(VertexData, VertexData::vertexPosition), sizeof(VertexData::vertexPosition), vk::Format::eR32G32B32Sfloat);
}

void AirEngine::Runtime::Graphic::MeshAttributePaser::Position_MeshAttributePaser::OnPopulateVertexData(void* vertexDataPtr, size_t vertexDataByteSize, const Asset::Mesh::MeshInfo& meshInfo, const aiScene& meshScene) const
{
	using TYPE = VertexData;

	for (uint32_t subMeshIndex = 0; subMeshIndex < meshInfo.meshCount; subMeshIndex++)
	{
		const auto& subMesh = meshScene.mMeshes[subMeshIndex];
		const auto& subMeshInfo = meshInfo.subMeshInfo[subMeshIndex];

		TYPE* subMeshVertexDataPtr = reinterpret_cast<TYPE*>(vertexDataPtr) + subMeshInfo.vertexOffset;

		for (uint32_t vertexIndex = 0; vertexIndex < subMeshInfo.vertexCount; vertexIndex++)
		{
			TYPE* vertexDataPtr = subMeshVertexDataPtr + vertexIndex;

			// positions
			{
				auto& position = vertexDataPtr->vertexPosition;
				auto& vertexPosition = subMesh->mVertices[vertexIndex];

				position.x = vertexPosition.x;
				position.y = vertexPosition.y;
				position.z = vertexPosition.z;
			}
		}
	}
}

#include "moc_Position_MeshAttributePaser.cpp"
//#include "Position_MeshAttributePaser.moc"