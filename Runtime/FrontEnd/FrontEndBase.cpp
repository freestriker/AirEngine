#include "FrontEndBase.hpp"

AirEngine::Runtime::FrontEnd::FrontEndBase::FrontEndBase()
	: FrontEndBase(false)
{
}

AirEngine::Runtime::FrontEnd::FrontEndBase::FrontEndBase(const bool isWindow)
	: _isWindow(isWindow)
{
}

AirEngine::Runtime::FrontEnd::WindowFrontEndBase::WindowFrontEndBase()
	: FrontEndBase(true)
	, _vkbSwapchain()
	, _vkSwapchain()
	, _vkSurface()
{
}
