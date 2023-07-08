#pragma once
#include "GarbageCollectInclude.hpp"
#include "ContructorMacro.hpp"
#include "ExportMacro.hpp"

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
		namespace Utility
		{
			class AIR_ENGINE_API CollectableObject
				: public gc
			{
			public:
				CollectableObject() = default;
				virtual ~CollectableObject() = default;
				NO_COPY_MOVE(CollectableObject)
			};
		}
	}
}