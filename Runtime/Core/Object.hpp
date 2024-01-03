#pragma once
#include <typeinfo>
#include <string>
#include <rttr/type>
#include <rttr/registration>
#include "AirEngine/Runtime/Utility/ContructorMacro.hpp"
#include "AirEngine/Runtime/Utility/ExportMacro.hpp"

#ifndef AIR_ENGINE_NEW

#define NEW_COLLECTABLE_OBJECT new(GC)
#define NEW_COLLECTABLE_PURE_OBJECT new(PointerFreeGC)
#define NEW_TRACKABLE_OBJECT new(NoGC)

#define AIR_ENGINE_NEW

#endif // !AIR_ENGINE_NEW

namespace AirEngine
{
	namespace Runtime
	{
		namespace Core
		{
			class AIR_ENGINE_API Object
			{
			public:
				Object();
				virtual ~Object();
				NO_COPY_MOVE(Object)

				rttr::type Type();

				RTTR_ENABLE()
			};
		}
	}
}