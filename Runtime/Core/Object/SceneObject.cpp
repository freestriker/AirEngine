#include "SceneObject.hpp"
#include <rttr/registration>

RTTR_REGISTRATION
{
	rttr::registration::class_<AirEngine::Runtime::Core::Object::SceneObject>("AirEngine::Runtime::Core::Object::SceneObject");
}

AirEngine::Runtime::Core::Object::SceneObject::SceneObject()
	: Object()
	, _transform{}
	, _components{}
{
}

AirEngine::Runtime::Core::Object::SceneObject::~SceneObject()
{
}

std::shared_ptr<AirEngine::Runtime::Core::Object::Component> AirEngine::Runtime::Core::Object::SceneObject::GetComponent(rttr::type type)
{
	auto iter = _components.find(type);
	if (iter != _components.end())
	{
		return iter->second;
	}
	return std::shared_ptr<Component>();
}
