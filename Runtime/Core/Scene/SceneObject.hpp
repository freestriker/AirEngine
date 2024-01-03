#pragma once
#include <string>
#include <rttr/registration>
#include "../../Utility/ContructorMacro.hpp"
#include "../../Utility/ExportMacro.hpp"
#include "../Object.hpp"
#include <glm/vec3.hpp>
#include <glm/gtc/quaternion.hpp>
#include <unordered_map>
#include <memory>
#include <string_view>
#include <boost/intrusive/list.hpp>
#include "../../Utility/ChildBrotherTreeNode.hpp"
#include "Component.hpp"

#include "../../Utility/BitOperationMacro.hpp"

namespace AirEngine
{
	namespace Runtime
	{
		namespace Core
		{
			namespace Scene
			{
				class Scene;
				class AIR_ENGINE_API SceneObject final
					: public Object
					, public AirEngine::Utility::ChildBrotherTreeNode<SceneObject>
				{
					friend class Scene;
				private:
					using SceneObjectMetaDataType = uint32_t;
					SceneObjectMetaDataType _sceneObjectMetaData;
					static constexpr SceneObjectMetaDataType IS_ACTIVE_BITS = 1u;
					std::string _name;
					Scene* _scene;
					std::vector<Component*> _components;
					glm::vec3 _translation;
					glm::quat _quaternion;
					glm::vec3 _scale;
					glm::mat4 _modelMatrix;
				public:
					static constexpr glm::mat4 BASE_MATRIX = glm::mat4(1);
					static constexpr glm::quat BASE_QUATERNION = glm::quat(1, 0, 0, 0);
					static constexpr glm::vec3 BASE_SCALE = glm::vec3(1);
					static constexpr glm::vec3 BASE_TRANSLATION = glm::vec3(0);
				public:
					SceneObject();
					SceneObject(const std::string_view& name, bool active);
					SceneObject(const std::string_view& name);
					~SceneObject();
					NO_COPY_MOVE(SceneObject)

					bool inline Active()const
					{
						return (_sceneObjectMetaData & IS_ACTIVE_BITS) == IS_ACTIVE_BITS;
					}
					void SetActive(bool active)
					{
						_sceneObjectMetaData = IF_SET_BITS(active, _sceneObjectMetaData, IS_ACTIVE_BITS);
					}
					inline std::string& Name()
					{
						return _name;
					}
					inline Scene& Scene()
					{
						return *_scene;
					}
					inline bool IsSettled()const
					{
						return _scene != nullptr;
					}
					inline bool IsVagrant()const
					{
						return _scene == nullptr;
					}

					void AttachComponent(Component& component);
					Component& GetComponent(rttr::type type);
					inline Component& GetComponent(const std::string_view& type);
					template<class TComponent>
					inline TComponent& GetComponent();
					void DetachComponent(Component& component);

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
					inline const glm::mat4& ModelMatrix() const
					{
						return _modelMatrix;
					}

					void SetTranslation(const glm::vec3& translation);
					void SetScale(const glm::vec3& scale);
					void SetQuaternion(const glm::vec3& quaternion);
					void SetTranslationQuaternionScale(const glm::vec3& translation, const glm::vec3& quaternion, const glm::vec3& scale);
				private:
					void OnAttachToTree()override;
					void OnDetachFromTree()override;
					void UpdateModelMatrix(const glm::mat4& parentModelMatrix);
					void AutoAttachToSettledParent();
					void AutoAttachToVagrantParent();
					void AutoDetachFromSettledParent();
					void AutoDetachFromVagrantParent();
					void AutoUpdatePosition();
					void AutoUpdatePosition(const glm::mat4& modelMatrix);

					RTTR_ENABLE(Object)
				};
				template<class TComponent>
				inline TComponent& SceneObject::GetComponent()
				{
					return static_cast<TComponent&>(GetComponent(rttr::type::get<TComponent>()));
				}

				inline Component& AirEngine::Runtime::Core::Scene::SceneObject::GetComponent(const std::string_view& type)
				{
					return GetComponent(rttr::type::get_by_name(rttr::string_view(type.data(), type.size())));
				}
			}
		}
	}
}