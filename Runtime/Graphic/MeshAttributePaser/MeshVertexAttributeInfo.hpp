#pragma once
#include <map>
#include <vulkan/vulkan.hpp>
#include "AirEngine/Runtime/Utility/InternedString.hpp"

namespace AirEngine
{
	namespace Runtime
	{
		namespace Graphic
		{
			namespace MeshAttributePaser
			{
				struct MeshVertexAttributeInfo
				{
					Utility::InternedString name;
					uint16_t offset;
					uint16_t size;
					vk::Format format;
				};
				using MeshVertexAttributeInfoMap = std::map<Utility::InternedString, MeshVertexAttributeInfo> ;
			}
		}
	}
}