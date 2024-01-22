#include "Mesh.hpp"

AirEngine::Runtime::Graphic::Asset::Mesh::Mesh()
	: AssetBase()
	, _vertexBuffer(nullptr)
	, _indexBuffer(nullptr)
{
}

AirEngine::Runtime::Graphic::Asset::Mesh::~Mesh()
{
	delete _vertexBuffer;
	delete _indexBuffer;
}