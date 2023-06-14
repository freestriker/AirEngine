﻿#include "Mesh.hpp"
#include <rttr/registration>

RTTR_REGISTRATION
{
	rttr::registration::class_<AirEngine::Runtime::Asset::Mesh>("AirEngine::Runtime::Asset::Mesh");
}

AirEngine::Runtime::Asset::Mesh::Mesh()
	: AssetBase()
	, _vertexBuffer(nullptr)
	, _indexBuffer(nullptr)
	, _indexType(VkIndexType::VK_INDEX_TYPE_UINT16)
	, _perIndexByteCount(0)
{
}

AirEngine::Runtime::Asset::Mesh::~Mesh()
{
	delete _vertexBuffer;
	_vertexBuffer = nullptr;
	if (_indexBuffer != nullptr)
	{
		delete _indexBuffer;
		_indexBuffer = nullptr;
	}
}