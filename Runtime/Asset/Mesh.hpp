#pragma once
#include "AssetBase.hpp"
#include "../Utility/InternedString.hpp"
#include <glm/vec2.hpp>
#include <glm/vec3.hpp>
#include <vulkan/vulkan_core.h>

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
			private:
				Graphic::Instance::Buffer* _vertexBuffer;
				Graphic::Instance::Buffer* _indexBuffer;
				std::vector<SubMeshInfo> _subMeshInfos;
				VkIndexType _indexType;
				uint8_t _perIndexByteCount;
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
				inline VkIndexType IndexType()const
				{
					return _indexType;
				}
				inline uint8_t PerIndexByteCount()const
				{
					return _perIndexByteCount;
				}
			};
		}
	}
}