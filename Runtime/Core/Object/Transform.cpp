#include "Transform.hpp"
#include <rttr/registration>

RTTR_REGISTRATION
{
	rttr::registration::class_<AirEngine::Runtime::Core::Object::Transform>("AirEngine::Runtime::Core::Object::Transform");
}

AirEngine::Runtime::Core::Object::Transform::Transform()
	: ChildBrotherTreeNode()
	, Component(true, nullptr)
	, _translation{}
	, _quaternion{}
	, _scale{}
	, _modelMatrix{1}
{
}
