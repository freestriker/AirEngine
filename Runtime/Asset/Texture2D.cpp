#include "Texture2D.hpp"
#include <rttr/registration>

RTTR_REGISTRATION
{
	rttr::registration::class_<AirEngine::Runtime::Asset::Texture2D>("AirEngine::Runtime::Asset::Texture2D");
}

AirEngine::Runtime::Asset::Texture2D::Texture2D()
	: AssetBase()
	, _image(nullptr)
{

}

AirEngine::Runtime::Asset::Texture2D::~Texture2D()
{
	delete _image;
	_image = nullptr;
}
