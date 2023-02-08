#pragma once
#include <string>
#include <rttr/registration>
#include "../../Utility/ContructorMacro.hpp"
#include "../../Utility/ExportMacro.hpp"
#include "Object.hpp"
#include "Transform.hpp"
#include <unordered_map>
#include <memory>
#include <string_view>

namespace AirEngine
{
	namespace Runtime
	{
		namespace Core
		{
			namespace Object
			{
				class DLL_API SceneObject final
					: public Object
				{
				private:
					std::string _name;
					Transform _transform;
					std::unordered_multimap<rttr::type, std::shared_ptr<Component>> _components;
				public:
					SceneObject();
					~SceneObject();
					NO_COPY_MOVE(SceneObject)

					inline Transform& Transform()
					{
						return _transform;
					}
					inline std::string& Name()
					{
						return _name;
					}
					std::shared_ptr<Component> GetComponent(rttr::type type);
					inline std::shared_ptr<Component> GetComponent(const std::string_view& type);
					template<class TComponent>
					inline std::shared_ptr<TComponent> GetComponent();
					RTTR_ENABLE(Object)
				};
				template<class TComponent>
				inline std::shared_ptr<TComponent> SceneObject::GetComponent()
				{
					return std::static_pointer_cast<TComponent>(GetComponent(rttr::type::get<TComponent>()));
				}

				inline std::shared_ptr<Component> AirEngine::Runtime::Core::Object::SceneObject::GetComponent(const std::string_view& type)
				{
					return GetComponent(rttr::type::get_by_name(rttr::string_view(type.data(), type.size())));
				}
			}
		}
	}
}