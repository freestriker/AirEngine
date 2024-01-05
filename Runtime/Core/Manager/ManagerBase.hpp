#pragma once
#include "AirEngine/Runtime/Utility/ExportMacro.hpp"
#include "AirEngine/Runtime/Utility/ContructorMacro.hpp"
#include <string>
#include <vector>
#include "AirEngine/Runtime/Utility/OperationWrapper.hpp"
#include <iostream>

namespace AirEngine
{
	namespace Runtime
	{
		namespace Core
		{
			namespace Manager
			{
				class MainManager;
				class AIR_ENGINE_API ManagerBase
				{
					friend class MainManager;
				private:
					const std::string _name;
				protected:
					ManagerBase(const std::string& name);
					virtual ~ManagerBase() = default;
					NO_COPY_MOVE(ManagerBase)

					virtual Utility::OperationWrapper OnGetPreInitializeOperation()
					{
						return { };
					}
					virtual std::vector<Utility::OperationWrapper> OnGetInitializeOperations()
					{
						return { };
					}
					virtual Utility::OperationWrapper OnGetPostInitializeOperation()
					{
						return { };
					}

					virtual std::vector<Utility::OperationWrapper> OnGetUpdateOperations()
					{
						return { };
					}

					virtual Utility::OperationWrapper OnGetPreFinalizeOperation()
					{
						return { };
					}
					virtual std::vector<Utility::OperationWrapper> OnGetFinalizeOperations()
					{
						return { };
					}
					virtual Utility::OperationWrapper OnGetPostFinalizeOperation()
					{
						return { };
					}
				public:
					const std::string& Name() const
					{
						return _name;
					}

					//Initialize
					static constexpr uint32_t PRE_INITIALIZE_LAYER = 0;

					static constexpr uint32_t GRAPHIC_INITIALIZE_LAYER = 100;
					static constexpr uint32_t GRAPHIC_INITIALIZE_INSTANCE_INDEX = 0;
					static constexpr uint32_t GRAPHIC_INITIALIZE_WINDOW_SURFACE_INDEX = 100;
					static constexpr uint32_t GRAPHIC_INITIALIZE_DEVICE_INDEX = 200;
					static constexpr uint32_t GRAPHIC_INITIALIZE_SWAPCHAIN_INDEX = 300;
					static constexpr uint32_t GRAPHIC_INITIALIZE_ORTHER_INDEX = 400;

					static constexpr uint32_t POST_INITIALIZE_LAYER = 1000;

					//Update
					static constexpr uint32_t LOGIC_UPDATE_LAYER = 0;
					static constexpr uint32_t RENDER_UPDATE_LAYER = 100;
					static constexpr uint32_t COLLECT_UPDATE_LAYER = 1000;

				};
			}
		}
	}
}

