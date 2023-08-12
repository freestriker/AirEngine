#pragma once
#include "AssetBase.hpp"
#include "../Utility/InternedString.hpp"
#include <glm/vec2.hpp>
#include <glm/vec3.hpp>
#include <vulkan/vulkan.hpp>

namespace AirEngine
{
	namespace Runtime
	{
		namespace Graphic
		{
			namespace Instance
			{
				class Buffer;
			}
		}
		namespace AssetLoader
		{
			class MeshLoader;
		}
		namespace Asset
		{
			class AIR_ENGINE_API Mesh final
				: public AssetBase
			{
				friend class AssetLoader::MeshLoader;
			public:
				struct VertexData
				{
					glm::vec3 position;
					glm::vec2 texCoords;
					glm::vec3 normal;
					glm::vec3 tangent;
				};
				struct SubMeshInfo
				{
					Utility::InternedString name;
					uint32_t vertexOffset;
					uint32_t vertexCount;
					uint32_t indexOffset;
					uint32_t indexCount;
				};
				struct MeshInfo
				{
					std::vector<SubMeshInfo> subMeshInfo;
					uint32_t vertexCount;
					uint32_t indexCount;
					uint32_t meshCount;
					vk::IndexType indexType;
				};
			private:
				Graphic::Instance::Buffer* _vertexBuffer;
				Graphic::Instance::Buffer* _indexBuffer;
				MeshInfo _meshInfo;
			public:
				Mesh();
				~Mesh();
				NO_COPY_MOVE(Mesh);

				inline Graphic::Instance::Buffer& VertexBuffer()const
				{
					return *_vertexBuffer;
				}
				inline Graphic::Instance::Buffer& IndexBuffer()const
				{
					return *_indexBuffer;
				}
				inline const MeshInfo& Info()const
				{
					return _meshInfo;
				}
				static uint8_t IndexTypeToByteCount(vk::IndexType indexType)
				{
					switch (indexType)
					{
					case vk::IndexType::eUint8EXT:
						return 1;
					case vk::IndexType::eUint16:
						return 2;
					case vk::IndexType::eUint32:
						return 4;
					default:
						return 0;
					}
				}
			};
		}
	}
}