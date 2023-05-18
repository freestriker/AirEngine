#pragma once
#include <string>
#include <rttr/registration>
#include "../../Utility/ContructorMacro.hpp"
#include "../../Utility/ExportMacro.hpp"
#include "../Object.hpp"
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
					std::unordered_map <Component::ComponentCoarsnessType, ComponentLinkedList> _componentListMap;
					SceneObject _scene;
				public:
					Scene(const std::string& name);
					~Scene();
					NO_COPY_MOVE(Scene)

					inline const std::string& Name()const
					{
						return _name;
					}
					inline std::unordered_map<Component::ComponentCoarsnessType, ComponentLinkedList>& CcomponentListMap()
					{
						return _componentListMap;
					}
					inline SceneObject& RootSceneObject()
					{
						return _scene;
					}

					RTTR_ENABLE(Object)
				};
			}
		}
	}
}