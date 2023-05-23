#pragma once
#include "../../Utility/ExportMacro.hpp"
#include "../../Utility/ContructorMacro.hpp"
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
					void OnPreparePresent() override;
					void OnPresent() override;
					void OnFinishPresent() override;
				public:
					NO_COPY_MOVE(DummyWindow)
					DummyWindow();
					~DummyWindow() = default;
				};
			}
		}
	}
}