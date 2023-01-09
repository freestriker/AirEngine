#include "ManagerBase.hpp"

AirEngine::Runtime::Core::Manager::ManagerBase::ManagerBase(const std::string& name)
	: _name(name)
{
}

AirEngine::Runtime::Core::Manager::ManagerBase::~ManagerBase()
{
}

const std::string& AirEngine::Runtime::Core::Manager::ManagerBase::Name() const
{
	return _name;
}
