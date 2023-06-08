#pragma once
#include "../Utility/GarbageCollectInclude.hpp"
#include <typeinfo>
#include <string>
#include <rttr/type>
#include <rttr/registration>
#include "../Utility/ContructorMacro.hpp"
#include "../Utility/ExportMacro.hpp"

#ifndef AIR_ENGINE_NEW

#define NEW_COLLECTABLE_OBJECT new(GC)
#define NEW_TRACKABLE_OBJECT new(NoGC)

#endif // !AIR_ENGINE_NEW

namespace AirEngine
{
	namespace Runtime
	{
		namespace Core
		{
			class AIR_ENGINE_API Object
				: public gc
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