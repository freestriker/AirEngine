#pragma once
#include "AirEngine/Runtime/Utility/ContructorMacro.hpp"
#include "AirEngine/Runtime/Utility/ExportMacro.hpp"
#include <assimp/scene.h>
#include "AirEngine/Runtime/Asset/Mesh.hpp"
#include "AirEngine/Runtime/Utility/ReflectableObject.hpp"

namespace AirEngine
{
	namespace Runtime
	{
		namespace AssetLoader
		{
			class MeshLoader;
		}
		namespace Graphic
		{
			namespace MeshAttributePaser
			{
				class AIR_ENGINE_API MeshAttributePaserBase
					: public Utility::ReflectableObject
				{
					friend class AssetLoader::MeshLoader;
					REFLECTABLE_OBJECT
				protected:
					virtual void OnEditPostProcessSteps(uint32_t& postProcessSteps) const { };
					virtual uint32_t OnGetPerVertexByteSize() const = 0;
					virtual void OnPopulateMeshVertexAttributeInfoMap(std::map<Utility::InternedString, Asset::Mesh::MeshVertexAttributeInfo>& meshVertexAttributeInfoMap) const = 0;
					virtual void OnPopulateVertexData(void* vertexDataPtr, size_t vertexDataByteSize, const Asset::Mesh::MeshInfo& meshInfo, const aiScene& meshScene) const = 0;
				public:
					MeshAttributePaserBase() = default;
					virtual ~MeshAttributePaserBase() = default;
					NO_COPY_MOVE(MeshAttributePaserBase)
				};
			}
		}
	}
}