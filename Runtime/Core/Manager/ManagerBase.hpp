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
					static constexpr uint32_t GRAPHIC_PRE_INITIALIZE_INDEX = 0;
					static constexpr uint32_t GRAPHIC_PRE_INITIALIZE_INSTANCE_INDEX = 100;
					static constexpr uint32_t GRAPHIC_INITIALIZE_INSTANCE_INDEX = 200;
					static constexpr uint32_t GRAPHIC_POST_INITIALIZE_INSTANCE_INDEX = 300;
					static constexpr uint32_t GRAPHIC_PRE_INITIALIZE_WINDOW_SURFACE_INDEX = 400;
					static constexpr uint32_t GRAPHIC_INITIALIZE_WINDOW_SURFACE_INDEX = 500;
					static constexpr uint32_t GRAPHIC_POST_INITIALIZE_WINDOW_SURFACE_INDEX = 600;
					static constexpr uint32_t GRAPHIC_PRE_INITIALIZE_DEVICE_INDEX = 700;
					static constexpr uint32_t GRAPHIC_INITIALIZE_DEVICE_INDEX = 800;
					static constexpr uint32_t GRAPHIC_POST_INITIALIZE_DEVICE_INDEX = 900;
					static constexpr uint32_t GRAPHIC_PRE_INITIALIZE_SWAPCHAIN_INDEX = 1000;
					static constexpr uint32_t GRAPHIC_INITIALIZE_SWAPCHAIN_INDEX = 1100;
					static constexpr uint32_t GRAPHIC_POST_INITIALIZE_SWAPCHAIN_INDEX = 1200;
					static constexpr uint32_t GRAPHIC_POST_INITIALIZE_INDEX = 1300;

					static constexpr uint32_t POST_INITIALIZE_LAYER = 10000;

					//Update
					static constexpr uint32_t LOGIC_UPDATE_LAYER = 0;
					static constexpr uint32_t RENDER_UPDATE_LAYER = 100;
					static constexpr uint32_t COLLECT_UPDATE_LAYER = 1000;

				};
			}
		}
	}
}

