#include "Material.hpp"

AirEngine::Runtime::Graphic::Rendering::Material::Material(const Shader& shader)
	: _shader(&shader)
	, _bindableAssetInfoMap()
	, _descriptorSetMemoryInfosMap()
{
}

AirEngine::Runtime::Graphic::Rendering::Material::~Material()
{
}
