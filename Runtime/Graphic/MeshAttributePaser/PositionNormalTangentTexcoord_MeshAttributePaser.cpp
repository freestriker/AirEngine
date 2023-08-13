#include "PositionNormalTangentTexcoord_MeshAttributePaser.hpp"
#include <assimp/postprocess.h>

REGISTRATION
{
	DECLARE_TYPE(AirEngine::Runtime::Graphic::MeshAttributePaser::PositionNormalTangentTexcoord_MeshAttributePaser*)
}

struct VertexData
{
	glm::vec3 position;
	glm::vec2 texcoords;
	glm::vec3 normal;
	glm::vec3 tangent;
};

void AirEngine::Runtime::Graphic::MeshAttributePaser::PositionNormalTangentTexcoord_MeshAttributePaser::OnEditPostProcessSteps(uint32_t& postProcessSteps) const
{
	postProcessSteps |= aiProcess_GenNormals;
	postProcessSteps |= aiProcess_CalcTangentSpace;
	postProcessSteps |= aiProcess_GenUVCoords;
}

uint32_t AirEngine::Runtime::Graphic::MeshAttributePaser::PositionNormalTangentTexcoord_MeshAttributePaser::OnGetPerVertexByteSize() const
{
	return sizeof(VertexData);
}

void AirEngine::Runtime::Graphic::MeshAttributePaser::PositionNormalTangentTexcoord_MeshAttributePaser::OnPopulateVertexData(void* vertexDataPtr, size_t vertexDataByteSize, const Asset::Mesh::MeshInfo& meshInfo, const aiScene& meshScene) const
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
				auto& texCoords = vertexDataPtr->texcoords;
				auto& vertexTexCoords = subMesh->mTextureCoords[0][vertexIndex];

				texCoords.x = vertexTexCoords.x;
				texCoords.y = vertexTexCoords.y;
			}
		}
	}
}

#include "moc_PositionNormalTangentTexcoord_MeshAttributePaser.cpp"
#include "PositionNormalTangentTexcoord_MeshAttributePaser.moc"