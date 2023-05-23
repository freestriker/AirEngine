#include "FrontEndBase.hpp"

AirEngine::Runtime::Core::FrontEnd::FrontEndBase::FrontEndBase()
	: FrontEndBase(false)
{
}

AirEngine::Runtime::Core::FrontEnd::FrontEndBase::FrontEndBase(const bool isWindow)
	: _isWindow(isWindow)
{
}

AirEngine::Runtime::Core::FrontEnd::WindowFrontEndBase::WindowFrontEndBase()
	: FrontEndBase(true)
	, _vkbSwapchain()
	, _vkSwapchain(VK_NULL_HANDLE)
	, _vkSurface(VK_NULL_HANDLE)
{
}
