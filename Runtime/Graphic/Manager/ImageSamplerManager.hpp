#pragma once
#include "AirEngine/Runtime/Core/Manager/ManagerBase.hpp"
#include "AirEngine/Runtime/Utility/ContructorMacro.hpp"
#include "AirEngine/Runtime/Utility/ExportMacro.hpp"
#include <vulkan/vulkan.hpp>
#include <unordered_map>

namespace AirEngine
{
	namespace Runtime
	{
		namespace Graphic
		{
			namespace Instance
			{
				class ImageSampler;
			}
			namespace Manager
			{
				class AIR_ENGINE_API ImageSamplerManager final
					: public Core::Manager::ManagerBase
				{
				private:
					static void Initialize();
					virtual std::vector<Utility::OperationWrapper> OnGetInitializeOperations() override;
				private:
					static std::unordered_map<uint64_t, Instance::ImageSampler*> _imageSamplerMap;
				public:
					ImageSamplerManager();
					~ImageSamplerManager() override;
					NO_COPY_MOVE(ImageSamplerManager)

					static Instance::ImageSampler* ImageSampler(vk::Filter filter, vk::SamplerMipmapMode mipmapMode, vk::SamplerAddressMode addressMode, float minMipmapLevel, float maxMipmapLevel);
				};
			}
		}
	}
}