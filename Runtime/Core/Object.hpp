#pragma once
#include <typeinfo>
#include <string>
#include <rttr/type>
#include <rttr/registration>
#include "../Utility/ContructorMacro.hpp"
#include "../Utility/ExportMacro.hpp"

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