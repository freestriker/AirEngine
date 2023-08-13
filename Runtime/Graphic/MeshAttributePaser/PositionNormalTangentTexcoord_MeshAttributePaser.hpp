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
				class AIR_ENGINE_API PositionNormalTangentTexcoord_MeshAttributePaser final 
					: public MeshAttributePaserBase
				{
					REFLECTABLE_OBJECT
				protected:
					virtual void OnEditPostProcessSteps(uint32_t& postProcessSteps) const override;
					virtual uint32_t OnGetPerVertexByteSize() const override;
					virtual void OnPopulateVertexData(void* vertexDataPtr, size_t vertexDataByteSize, const Asset::Mesh::MeshInfo& meshInfo, const aiScene& meshScene) const override;
				public:
					INVOKABLE PositionNormalTangentTexcoord_MeshAttributePaser() = default;
					INVOKABLE~PositionNormalTangentTexcoord_MeshAttributePaser() = default;
					NO_COPY_MOVE(PositionNormalTangentTexcoord_MeshAttributePaser)
				};
			}
		}
	}
}