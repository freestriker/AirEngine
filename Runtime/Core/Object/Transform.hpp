#pragma once
#include <typeinfo>
#include <string>
#include <rttr/type>
#include <rttr/registration>
#include "../../Utility/ExportMacro.hpp"
#include "../../Utility/ContructorMacro.hpp"
#include <glm/vec3.hpp>
#include <glm/gtc/quaternion.hpp>
#include "../../Utility/ChildBrotherTreeNode.hpp"
#include "Component.hpp"

namespace AirEngine
{
	namespace Runtime
	{
		namespace Core
		{
			namespace Object
			{
				class DLL_API Transform final
					: public AirEngine::Utility::ChildBrotherTreeNode<Transform>
					, public Component
				{
				private:
					glm::vec3 _translation;
					glm::quat _quaternion;
					glm::vec3 _scale;
					glm::mat4 _modelMatrix;
				public:
					Transform();
					~Transform() = default;
					NO_COPY_MOVE(Transform)

					inline const glm::vec3& Translation() const
					{
						return _translation;
					}
					inline const glm::quat& Quaternion() const
					{
						return _quaternion;
					}
					inline const glm::vec3& Scale() const
					{
						return _scale;
					}

					RTTR_ENABLE(Component)
				};
			}
		}
	}
}