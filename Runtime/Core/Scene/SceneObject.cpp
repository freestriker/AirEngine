#include "SceneObject.hpp"
#include <rttr/registration>
#include "Component.hpp"
#include "Scene.hpp"
#include <cassert>

RTTR_REGISTRATION
{
	rttr::registration::class_<AirEngine::Runtime::Core::Scene::SceneObject>("AirEngine::Runtime::Core::Scene::SceneObject");
}

AirEngine::Runtime::Core::Scene::SceneObject::SceneObject()
	: SceneObject("")
{

}

AirEngine::Runtime::Core::Scene::SceneObject::SceneObject(const std::string_view& name, bool active, bool isManualUpdate)
	: Object()
	, ChildBrotherTreeNode()
	, _scene()
	, _name(name)
	, _components{}
	, _translation{ BASE_TRANSLATION }
	, _quaternion{ BASE_QUATERNION }
	, _scale{ BASE_SCALE }
	, _modelMatrix{ BASE_MATRIX }
	, _sceneObjectMetaData(
		IF_SET_BITS(isManualUpdate, 0, IS_MANUAL_UPDATE_BITS) |
		IF_SET_BITS(active, 0, IS_ACTIVE_BITS)
	)
{
}

AirEngine::Runtime::Core::Scene::SceneObject::SceneObject(const std::string_view& name, bool isManuallyUpdated)
	: SceneObject(name, true, isManuallyUpdated)
{
}

AirEngine::Runtime::Core::Scene::SceneObject::SceneObject(const std::string_view& name)
	: SceneObject(name, true, false)
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
	assert(component._sceneObject == nullptr);

	component._sceneObject = this;
	_components.emplace_back(&component);

	if (component.IsSceneDependent() && IsSettled())
	{
		component.OnAttachToScene();
	}

	if (component.IsSceneObjectDependent())
	{
		component.OnAttachToSceneObject();
	}

	//if (IsManualUpdate()) return;

	if (component.IsPositionDependent()) 
	{
		component.OnChangePosition();
	}
}

void AirEngine::Runtime::Core::Scene::SceneObject::DetachComponent(Component& component)
{
	assert(component._sceneObject == this);

	for (auto iter = _components.begin(); iter != _components.end(); ++iter)
	{
		if (*iter == &component)
		{
			if (component.IsSceneObjectDependent())
			{
				component.OnDetachFromSceneObject();
			}

			if (component.IsSceneDependent() && IsSettled())
			{
				component.OnDetachFromScene();
			}

			component._sceneObject = nullptr;
			_components.erase(iter);
			break;
		}
	}
}

void AirEngine::Runtime::Core::Scene::SceneObject::SetTranslation(const glm::vec3& translation)
{
	_translation = translation;

	AutoUpdatePosition();
}

void AirEngine::Runtime::Core::Scene::SceneObject::SetScale(const glm::vec3& scale)
{
	_scale = scale;

	AutoUpdatePosition();
}

void AirEngine::Runtime::Core::Scene::SceneObject::SetQuaternion(const glm::vec3& quaternion)
{
	_quaternion = quaternion;

	AutoUpdatePosition();
}

void AirEngine::Runtime::Core::Scene::SceneObject::SetTranslationQuaternionScale(const glm::vec3& translation, const glm::vec3& quaternion, const glm::vec3& scale)
{
	_translation = translation;
	_scale = scale;
	_quaternion = quaternion;

	AutoUpdatePosition();
}
//
//void AirEngine::Runtime::Core::Scene::SceneObject::ManualUpdatePosition()
//{
//	ManualUpdatePosition(HaveParent() ? Parent().ModelMatrix() : BASE_MATRIX);
//}
//
//void AirEngine::Runtime::Core::Scene::SceneObject::ManualUpdatePosition(const glm::mat4& modelMatrix)
//{
//	UpdateModelMatrix(modelMatrix);
//	for (auto iter = ChildIterator(); iter; ++iter)
//	{
//		iter->ManualUpdatePosition(_modelMatrix);
//	}
//	for (auto& component : _components)
//	{
//		if (component->IsPositionDependent()) component->OnChangePosition();
//	}
//}

void AirEngine::Runtime::Core::Scene::SceneObject::OnAttachToTree()
{
	if (Parent().IsSettled())
	{
		AutoAttachToSettledParent();
	}
	else
	{
		AutoAttachToVagrantParent();
	}
}

void AirEngine::Runtime::Core::Scene::SceneObject::OnDetachFromTree()
{
	if (Parent().IsSettled())
	{
		AutoDetachFromSettledParent();
	}
	else
	{
		AutoDetachFromVagrantParent();
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

void AirEngine::Runtime::Core::Scene::SceneObject::AutoAttachToSettledParent()
{
	_scene = &Parent().Scene();
	UpdateModelMatrix(Parent().ModelMatrix());

	for (auto iter = ChildIterator(); iter; ++iter)
	{
		iter->AutoAttachToSettledParent();
	}
	for (auto& component : _components)
	{
		if (component->IsSceneDependent()) component->OnAttachToScene();

		if (component->IsPositionDependent()) component->OnChangePosition();
	}
	//_scene = &Parent().Scene();
	//if (!IsManualUpdate())
	//{
	//	UpdateModelMatrix(Parent().ModelMatrix());
	//}
	//for (auto iter = ChildIterator(); iter; ++iter)
	//{
	//	iter->AutoAttachToSettledParent();
	//}
	//for (auto& component : _components)
	//{
	//	if (component->IsSceneDependent()) component->OnAttachToScene();

	//	if (IsManualUpdate()) continue;
	//	if (component->IsPositionDependent()) component->OnChangePosition();
	//}
}

void AirEngine::Runtime::Core::Scene::SceneObject::AutoAttachToVagrantParent()
{
	UpdateModelMatrix(Parent().ModelMatrix());

	for (auto iter = ChildIterator(); iter; ++iter)
	{
		iter->AutoAttachToVagrantParent();
	}

	for (auto& component : _components)
	{
		if (component->IsPositionDependent()) component->OnChangePosition();
	}
	//if (IsManualUpdate()) return;

	//UpdateModelMatrix(Parent().ModelMatrix());

	//for (auto iter = ChildIterator(); iter; ++iter)
	//{
	//	iter->AutoAttachToVagrantParent();
	//}

	//for (auto& component : _components)
	//{
	//	if (component->IsPositionDependent()) component->OnChangePosition();
	//}
}

void AirEngine::Runtime::Core::Scene::SceneObject::AutoDetachFromSettledParent()
{
	for (auto& component : _components)
	{
		if (component->IsSceneDependent()) component->OnDetachFromScene();
	}
	for (auto iter = ChildIterator(); iter; ++iter)
	{
		iter->AutoDetachFromSettledParent();
	}
	_scene = nullptr;
}

void AirEngine::Runtime::Core::Scene::SceneObject::AutoDetachFromVagrantParent()
{
	for (auto iter = ChildIterator(); iter; ++iter)
	{
		iter->AutoDetachFromVagrantParent();
	}
}

void AirEngine::Runtime::Core::Scene::SceneObject::AutoUpdatePosition()
{
	//if (IsManualUpdate()) return;

	AutoUpdatePosition(HaveParent() ? Parent().ModelMatrix() : BASE_MATRIX);
}

void AirEngine::Runtime::Core::Scene::SceneObject::AutoUpdatePosition(const glm::mat4& modelMatrix)
{
	UpdateModelMatrix(modelMatrix);
	for (auto iter = ChildIterator(); iter; ++iter)
	{
		//if (iter->IsManualUpdate()) continue;
		iter->AutoUpdatePosition(_modelMatrix);
	}
	for (auto& component : _components)
	{
		if (component->IsPositionDependent()) component->OnChangePosition();
	}
}
