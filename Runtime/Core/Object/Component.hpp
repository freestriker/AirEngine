#pragma once
#include <typeinfo>
#include <string>
#include <rttr/type>
#include <rttr/registration>
#include "../../Utility/ContructorMacro.hpp"
#include "../../Utility/ExportMacro.hpp"
#include "Object.hpp"

namespace AirEngine
{
	namespace Runtime
	{
		namespace Core
		{
			namespace Object
			{
				class SceneObject;
				class DLL_API Component
					: public Object
				{
					friend class SceneObject;
				private:
				public:
					bool active;

				protected:
					SceneObject* _sceneObject;
					Component();
					Component(bool active);
					Component(bool active, AirEngine::Runtime::Core::Object::SceneObject* sceneObject);
					virtual ~Component() = default;
					NO_COPY_MOVE(Component)
				public:
					SceneObject* SceneObject();

					RTTR_ENABLE(Object)
				};
			}
		}
	}
}