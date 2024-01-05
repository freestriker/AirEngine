#pragma once
#include <functional>
#include <string>

namespace AirEngine
{
	namespace Runtime
	{
		namespace Utility
		{
			using Operation = std::function<void()>;

			struct OperationWrapper
			{
				uint32_t layer;
				uint32_t index;
				std::string description;
				Operation operation;
			};
		}
	}
}

