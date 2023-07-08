#include "Texture2D.hpp"

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
