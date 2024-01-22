#pragma once
#include "MeshAttributePaserBase.hpp"

namespace AirEngine
{
	namespace Runtime
	{
		namespace Graphic
		{
			namespace MeshAttributePaser
			{
				class AIR_ENGINE_API Position_MeshAttributePaser final 
					: public MeshAttributePaserBase
				{
					REFLECTABLE_OBJECT
				protected:
					void OnEditPostProcessSteps(uint32_t& postProcessSteps) const override;
					uint32_t OnGetPerVertexByteSize() const override;
					void OnPopulateMeshVertexAttributeInfoMap(MeshAttributePaser::MeshVertexAttributeInfoMap& meshVertexAttributeInfoMap) const override;
					void OnPopulateVertexData(void* vertexDataPtr, size_t vertexDataByteSize, const Asset::Mesh::MeshInfo& meshInfo, const aiScene& meshScene) const override;
				public:
					INVOKABLE Position_MeshAttributePaser() = default;
					INVOKABLE~Position_MeshAttributePaser() = default;
					NO_COPY_MOVE(Position_MeshAttributePaser)
				};
			}
		}
	}
}