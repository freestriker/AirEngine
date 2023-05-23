#pragma once
#include <functional>

namespace AirEngine
{
	namespace Runtime
	{
		namespace Utility
		{
			using Initializer = std::function<void()>;

			struct InitializerWrapper
			{
				uint32_t initLayer;
				uint32_t initIndex;
				Initializer initializer;
			};
		}
	}
}

