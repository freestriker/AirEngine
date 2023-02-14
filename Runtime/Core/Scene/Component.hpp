#pragma once
#include <typeinfo>
#include <string>
#include <rttr/type>
#include <rttr/registration>
#include "../../Utility/ContructorMacro.hpp"
#include "../../Utility/ExportMacro.hpp"
#include "../Object.hpp"
#include <boost/intrusive/list.hpp>

#define IF_SET_BITS(setOrNot, source, bits) ((setOrNot) ? (source) | (bits) : (source) & !(bits))

namespace AirEngine
{
	namespace Runtime
	{
		namespace Core
		{
			namespace Scene
			{
				class SceneObject;
				class DLL_API Component
					: public Object
				{
					friend class SceneObject;
				private:
					SceneObject* _sceneObject;
					using ComponentMetaDataType = uint32_t;
					ComponentMetaDataType _componentMetaDataType;
					static constexpr ComponentMetaDataType IS_ACTIVE_BITS = 1u;
					static constexpr ComponentMetaDataType IS_SCENE_DEPENDENT_BITS = 1u << 1u;
					static constexpr ComponentMetaDataType IS_SCENE_OBJECT_DEPENDENT_BITS = 1u << 2u;
					static constexpr ComponentMetaDataType IS_POSITION_DEPENDENT_BITS = 1u << 3u;
				public:
					using ComponentCoarsnessType = uint32_t;
					static constexpr Component::ComponentCoarsnessType COMPONENT_COARSNESS_TYPE = 0;
				protected:
					Component();
					Component(bool active);
					Component(bool isSceneDependent, bool isSceneObjectDependent, bool isPositionDependent, bool active);
					virtual ~Component() = default;
					virtual void OnAttachToScene();
					virtual void OnDetachFromScene();
					virtual void OnAttachToSceneObject();
					virtual void OnDetachFromSceneObject();
					virtual void OnSetActive(bool active);
					virtual void OnChangePosition();
					NO_COPY_MOVE(Component)
				public:
					inline bool HaveSceneObject()const
					{
						return _sceneObject != nullptr;
					}
					inline SceneObject& SceneObject()
					{
						return *_sceneObject;
					}

					bool inline Active()const
					{
						return _componentMetaDataType & IS_ACTIVE_BITS;
					}
					void SetActive(bool active)
					{
						OnSetActive(active);
						_componentMetaDataType = IF_SET_BITS(active, _componentMetaDataType, IS_ACTIVE_BITS);
					}
					bool IsSceneDependent() const
					{
						return _componentMetaDataType & IS_SCENE_DEPENDENT_BITS;
					}
					bool IsSceneObjectDependent() const
					{
						return _componentMetaDataType & IS_SCENE_OBJECT_DEPENDENT_BITS;
					}
					bool IsPositionDependent() const
					{
						return _componentMetaDataType & IS_POSITION_DEPENDENT_BITS;
					}
					RTTR_ENABLE(Object)
				};

				class ComponentLinkedNode
					: public boost::intrusive::list_base_hook<boost::intrusive::link_mode<boost::intrusive::auto_unlink>>
				{

				};
				using ComponentLinkedList = boost::intrusive::list<ComponentLinkedNode, boost::intrusive::constant_time_size<false>>;
			}
		}
	}
}