#include "SceneObject.hpp"
#include <rttr/registration>
#include "Component.hpp"
#include "Scene.hpp"

RTTR_REGISTRATION
{
	rttr::registration::class_<AirEngine::Runtime::Core::Scene::SceneObject>("AirEngine::Runtime::Core::Scene::SceneObject");
}

AirEngine::Runtime::Core::Scene::SceneObject::SceneObject()
	: SceneObject("")
{

}

AirEngine::Runtime::Core::Scene::SceneObject::SceneObject(const std::string_view& name)
	: Object()
	, ChildBrotherTreeNode()
	, _scene()
	, _name(name)
	, _components{}
	, _translation{ BASE_TRANSLATION }
	, _quaternion{ BASE_QUATERNION }
	, _scale{ BASE_SCALE }
	, _modelMatrix{ BASE_MATRIX }
{

}

AirEngine::Runtime::Core::Scene::SceneObject::~SceneObject()
{
}

AirEngine::Runtime::Core::Scene::Component& AirEngine::Runtime::Core::Scene::SceneObject::GetComponent(rttr::type type)
{
	for (auto& component : _components)
	{
		auto curType = component->Type();
		if (type == curType || type.is_base_of(curType))
		{
			return *component;
		}
	}
	return *static_cast<Component*>(nullptr);
}

void AirEngine::Runtime::Core::Scene::SceneObject::AttachComponent(Component& component)
{
	component._sceneObject = this;
	_components.emplace_back(&component);

	if (component.IsSceneDependent() && IsInScene())
	{
		component.OnAttachToScene();
	}

	if (component.IsSceneObjectDependent())
	{
		component.OnAttachToSceneObject();
	}

	if (component.IsPositionDependent()) 
	{
		component.OnChangePosition();
	}
}

void AirEngine::Runtime::Core::Scene::SceneObject::DetachComponent(Component& component)
{
	if (component._sceneObject == this)
	{
		for (auto iter = _components.begin(); iter != _components.end(); ++iter)
		{
			if (*iter == &component)
			{
				if (component.IsSceneObjectDependent())
				{
					component.OnDetachFromSceneObject();
				}

				if (component.IsSceneDependent() && IsInScene())
				{
					component.OnDetachFromScene();
				}

				component._sceneObject = nullptr;
				_components.erase(iter);
				break;
			}
		}
	}
}

void AirEngine::Runtime::Core::Scene::SceneObject::SetTranslation(const glm::vec3& translation)
{
	_translation = translation;

	AutoUpdateAllPosition();
}

void AirEngine::Runtime::Core::Scene::SceneObject::SetScale(const glm::vec3& scale)
{
	_scale = scale;

	AutoUpdateAllPosition();
}

void AirEngine::Runtime::Core::Scene::SceneObject::SetQuaternion(const glm::vec3& quaternion)
{
	_quaternion = quaternion;

	AutoUpdateAllPosition();
}

void AirEngine::Runtime::Core::Scene::SceneObject::SetTranslationQuaternionScale(const glm::vec3& translation, const glm::vec3& quaternion, const glm::vec3& scale)
{
	_translation = translation;
	_scale = scale;
	_quaternion = quaternion;

	AutoUpdateAllPosition();
}

void AirEngine::Runtime::Core::Scene::SceneObject::OnAttachToTree()
{
	if (Parent().IsInScene())
	{
		AutoUpdateAllAttachToSettledSceneObject();
	}
	else
	{
		AutoUpdateAllAttachToVagrantSceneObject();
	}
}

void AirEngine::Runtime::Core::Scene::SceneObject::OnDetachFromTree()
{
	if (Parent().IsInScene())
	{
		AutoUpdateAllDetachFromSettledSceneObject();
	}
	else
	{
		AutoUpdateAllDetachFromVagrantSceneObject();
	}
}

void AirEngine::Runtime::Core::Scene::SceneObject::UpdateModelMatrix(const glm::mat4& parentModelMatrix)
{
	glm::mat4 relativeModelMatrix = BASE_MATRIX;
	relativeModelMatrix[0][0] = _scale.x;
	relativeModelMatrix[1][1] = _scale.y;
	relativeModelMatrix[2][2] = _scale.z;

	relativeModelMatrix = glm::mat4_cast(_quaternion) * relativeModelMatrix;

	relativeModelMatrix = glm::translate(relativeModelMatrix, _translation);

	_modelMatrix = parentModelMatrix * relativeModelMatrix;
}

void AirEngine::Runtime::Core::Scene::SceneObject::AutoUpdateAllAttachToSettledSceneObject()
{
	_scene = &Parent().Scene();
	UpdateModelMatrix(Parent().ModelMatrix());
	for (auto iter = ChildIterator(); iter; ++iter)
	{
		iter->AutoUpdateAllAttachToSettledSceneObject(_modelMatrix);
	}
	for (auto& component : _components)
	{
		if (component->IsSceneDependent() && IsInScene()) component->OnAttachToScene();
		if (component->IsPositionDependent()) component->OnChangePosition();
	}
}

void AirEngine::Runtime::Core::Scene::SceneObject::AutoUpdateAllAttachToSettledSceneObject(const glm::mat4& parentModelMatrix)
{
	_scene = &Parent().Scene();
	UpdateModelMatrix(parentModelMatrix);
	for (auto iter = ChildIterator(); iter; ++iter)
	{
		iter->AutoUpdateAllAttachToSettledSceneObject(_modelMatrix);
	}
	for (auto& component : _components)
	{
		if (component->IsSceneDependent() && IsInScene()) component->OnAttachToScene();
		if (component->IsPositionDependent()) component->OnChangePosition();
	}
}

void AirEngine::Runtime::Core::Scene::SceneObject::AutoUpdateAllAttachToVagrantSceneObject()
{
	UpdateModelMatrix(Parent().ModelMatrix());
	for (auto iter = ChildIterator(); iter; ++iter)
	{
		iter->AutoUpdateAllAttachToVagrantSceneObject(_modelMatrix);
	}
	for (auto& component : _components)
	{
		if (component->IsPositionDependent()) component->OnChangePosition();
	}
}

void AirEngine::Runtime::Core::Scene::SceneObject::AutoUpdateAllAttachToVagrantSceneObject(const glm::mat4& parentModelMatrix)
{
	UpdateModelMatrix(parentModelMatrix);
	for (auto iter = ChildIterator(); iter; ++iter)
	{
		iter->AutoUpdateAllAttachToVagrantSceneObject(_modelMatrix);
	}
	for (auto& component : _components)
	{
		if (component->IsPositionDependent()) component->OnChangePosition();
	}
}

void AirEngine::Runtime::Core::Scene::SceneObject::AutoUpdateAllDetachFromSettledSceneObject()
{
	for (auto& component : _components)
	{
		if (component->IsSceneDependent() && IsInScene()) component->OnDetachFromScene();
	}
	for (auto iter = ChildIterator(); iter; ++iter)
	{
		iter->AutoUpdateAllDetachFromSettledSceneObject();
	}
	_scene = nullptr;
}

void AirEngine::Runtime::Core::Scene::SceneObject::AutoUpdateAllDetachFromVagrantSceneObject()
{
	for (auto iter = ChildIterator(); iter; ++iter)
	{
		iter->AutoUpdateAllDetachFromVagrantSceneObject();
	}
}

void AirEngine::Runtime::Core::Scene::SceneObject::AutoUpdateAllPosition()
{
	AutoUpdateAllPosition(HaveParent() ? Parent().ModelMatrix() : BASE_MATRIX);
}

void AirEngine::Runtime::Core::Scene::SceneObject::AutoUpdateAllPosition(const glm::mat4& modelMatrix)
{
	UpdateModelMatrix(modelMatrix);
	for (auto iter = ChildIterator(); iter; ++iter)
	{
		iter->AutoUpdateAllPosition(_modelMatrix);
	}
	for (auto& component : _components)
	{
		if (component->IsPositionDependent()) component->OnChangePosition();
	}
}
