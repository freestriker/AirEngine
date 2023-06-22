#pragma once
#include "../../Utility/ContructorMacro.hpp"
#include "../../Utility/ExportMacro.hpp"
#include "../../Utility/Fiber.hpp"
#include "../../Utility/InternedString.hpp"
#include <unordered_map>
#include <rttr/type>
#include <rttr/registration>

namespace AirEngine
{
	namespace Runtime
	{
		namespace Core
		{
			namespace Manager
			{
				class GraphicDeviceManager;
			}
		}
		namespace Graphic
		{
			namespace Instance
			{
				class RenderPassBase;
			}
			namespace Manager
			{
				class AIR_ENGINE_API RenderPassManager final
				{
					friend class Core::Manager::GraphicDeviceManager;
				private:
					struct ReferenceInfo
					{
						uint32_t refrenceCount;
						Utility::InternedString typeName;
						Instance::RenderPassBase* renderPass;
					};
					static Utility::Fiber::mutex _managerMutex;
					static std::unordered_map<Utility::InternedString, ReferenceInfo> _referenceMap;
				public:
					static Instance::RenderPassBase* LoadRenderPass(const std::string& renderPassTypeName);
					template<typename TRenderPass>
					static TRenderPass* LoadRenderPass();
					static void UnloadRenderPass(const std::string& renderPassTypeName);
					template<typename TRenderPass>
					static void UnloadRenderPass();
					static void Collect();
				private:
					RenderPassManager() = delete;
					~RenderPassManager() = delete;
					NO_COPY_MOVE(RenderPassManager)
				public:
				};

				template<typename TRenderPass>
				TRenderPass* AirEngine::Runtime::Graphic::Manager::RenderPassManager::LoadRenderPass()
				{
					return dynamic_cast<TRenderPass * >(LoadRenderPass(rttr::type::get<TRenderPass>().get_name().to_string()));
				}

				template<typename TRenderPass>
				void AirEngine::Runtime::Graphic::Manager::RenderPassManager::UnloadRenderPass()
				{
					UnloadRenderPass(rttr::type::get<TRenderPass>().get_name().to_string());
				}
			}
		}
	}
}