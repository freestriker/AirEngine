﻿#include "Mesh.hpp"

AirEngine::Runtime::Graphic::Asset::Mesh::Mesh()
	: AssetBase()
	, _vertexBuffer(nullptr)
	, _indexBuffer(nullptr)
{
}

AirEngine::Runtime::Graphic::Asset::Mesh::~Mesh()
{
	delete _vertexBuffer;
	_vertexBuffer = nullptr;
	if (_indexBuffer != nullptr)
	{
		delete _indexBuffer;
		_indexBuffer = nullptr;
	}
}