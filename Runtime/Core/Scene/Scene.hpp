#pragma once
#include <string>
#include <rttr/registration>
#include "AirEngine/Runtime/Utility/ContructorMacro.hpp"
#include "AirEngine/Runtime/Utility/ExportMacro.hpp"
#include "AirEngine/Runtime/Core/Object.hpp"
#include <unordered_map>
#include <memory>
#include <string_view>
#include <boost/intrusive/list.hpp>
#include "SceneObject.hpp"
#include "Component.hpp"

namespace AirEngine
{
	namespace Runtime
	{
		namespace Core
		{
			namespace Scene
			{
				class AIR_ENGINE_API Scene final
					: public Object
				{
				private:
					const std::string _name;
					SceneObject _rootSceneObject;
				public:
					Scene(const std::string& name);
					~Scene();
					NO_COPY_MOVE(Scene)

					inline const std::string& Name()const
					{
						return _name;
					}
					inline SceneObject& RootSceneObject()
					{
						return _rootSceneObject;
					}

					RTTR_ENABLE(Object)
				};
			}
		}
	}
}