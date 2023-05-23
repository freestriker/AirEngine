#pragma once
#include "../../Utility/ExportMacro.hpp"
#include "../../Utility/ContructorMacro.hpp"
#include <qwindow.h>
#include "FrontEndBase.hpp"
#include <qvulkaninstance.h>

namespace AirEngine
{
	namespace Runtime
	{
		namespace Core
		{
			namespace FrontEnd
			{
				class AIR_ENGINE_API Window
					: public QWindow
					, public WindowFrontEndBase
				{
				private:
					void OnPreparePresent() override;
					void OnPresent() override;
					void OnFinishPresent() override;
					void OnSetVulkanHandle() override;
					void OnCreateVulkanSwapchain() override;
					QVulkanInstance _qVulkanInstance;
				public:
					NO_COPY_MOVE(Window)
					Window();
					~Window() = default;
					inline QVulkanInstance& QVulkanInstance()
					{
						return _qVulkanInstance;
					}
				};
			}
		}
	}
}