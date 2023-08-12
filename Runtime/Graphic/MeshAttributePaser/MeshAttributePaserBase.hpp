#pragma once
#include "../../Utility/ContructorMacro.hpp"
#include "../../Utility/ExportMacro.hpp"
#include <assimp/scene.h>
#include "../../Asset/Mesh.hpp"

namespace AirEngine
{
	namespace Runtime
	{
		namespace Graphic
		{
			namespace MeshAttributePaser
			{
				class AIR_ENGINE_API MeshAttributePaserBase
				{
				protected:
					virtual void OnEditPostProcessSteps(uint32_t& postProcessSteps) const { };
					virtual uint32_t OnGetPerVertexByteSize() const = 0;
					virtual void OnPopulateVertexData(void* vertexDataPtr, size_t vertexDataByteSize, const Asset::Mesh::MeshInfo& meshInfo, const aiScene& meshScene) const = 0;
					MeshAttributePaserBase() = default;
					~MeshAttributePaserBase() = default;
				public:
					NO_COPY_MOVE(MeshAttributePaserBase)
				};
			}
		}
	}
}