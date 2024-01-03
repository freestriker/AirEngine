#pragma once
#include "AirEngine/Runtime/Utility/ExportMacro.hpp"
#include "AirEngine/Runtime/Utility/ContructorMacro.hpp"
#include <qwindow.h>
#include "FrontEndBase.hpp"

namespace AirEngine
{
	namespace Runtime
	{
		namespace Core
		{
			namespace FrontEnd
			{
				class AIR_ENGINE_API DummyWindow
					: public FrontEndBase
				{
				private:
					void OnCreate() override;
					void OnAcquireImage() override;
					void OnPresent() override;
				public:
					NO_COPY_MOVE(DummyWindow)
					DummyWindow();
					~DummyWindow() = default;
				};
			}
		}
	}
}