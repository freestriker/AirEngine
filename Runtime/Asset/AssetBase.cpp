#include "AssetBase.hpp"
#include <rttr/registration>

RTTR_REGISTRATION
{
	rttr::registration::class_<AirEngine::Runtime::Asset::AssetBase>("AirEngine::Runtime::Asset::AssetBase");
}

AirEngine::Runtime::Asset::AssetBase::AssetBase()
	: Core::Object()
	, assetLoadContext(nullptr)
{
}

AirEngine::Runtime::Asset::AssetBase::~AssetBase()
{
	assetLoadContext = nullptr;
}
