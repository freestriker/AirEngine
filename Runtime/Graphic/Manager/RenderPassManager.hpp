#pragma once
#include "AirEngine/Runtime/Core/Manager/ManagerBase.hpp"
#include "AirEngine/Runtime/Utility/ContructorMacro.hpp"
#include "AirEngine/Runtime/Utility/ExportMacro.hpp"
#include "AirEngine/Runtime/Utility/InternedString.hpp"
#include <unordered_map>
#include "AirEngine/Runtime/Utility/ReflectableObject.hpp"

namespace AirEngine
{
	namespace Runtime
	{
		namespace Graphic
		{
			namespace Instance
			{
				class RenderPassBase;
			}
			namespace Manager
			{
				class AIR_ENGINE_API RenderPassManager final
					: public AirEngine::Runtime::Core::Manager::ManagerBase
				{
				private:
					struct ReferenceInfo
					{
						uint32_t refrenceCount;
						Utility::InternedString typeName;
						Instance::RenderPassBase* renderPass;
					};
					static std::mutex _managerMutex;
					static std::unordered_map<Utility::InternedString, ReferenceInfo> _referenceMap;
					static Instance::RenderPassBase* LoadRenderPassImpl(const Utility::InternedString renderPassTypeName, int typeId);
					static void UnloadRenderPassImpl(const Utility::InternedString renderPassTypeName);
					virtual std::vector<Utility::OperationWrapper> OnGetUpdateOperations() override;
				public:
					static inline Instance::RenderPassBase* LoadRenderPass(const std::string& renderPassTypeName)
					{
						std::string ptrTypeName(renderPassTypeName + "*");
						int typeId = Utility::MetaType::type(ptrTypeName.c_str());

						return LoadRenderPassImpl(Utility::InternedString::InternedString(ptrTypeName), typeId);
					}
					template<typename TRenderPass>
					static inline TRenderPass* LoadRenderPass()
					{
						int typeId = Utility::MetaType::fromType<TRenderPass*>().id();
						std::string_view ptrTypeName(Utility::MetaType::typeName(typeId));

						return dynamic_cast<TRenderPass * >(LoadRenderPassImpl(Utility::InternedString::InternedString(ptrTypeName), typeId));
					}
					static void UnloadRenderPass(const std::string& renderPassTypeName)
					{
						UnloadRenderPassImpl(Utility::InternedString::InternedString(std::string(renderPassTypeName + "*")));
					}
					template<typename TRenderPass>
					static void UnloadRenderPass()
					{
						int typeId = Utility::MetaType::fromType<TRenderPass*>().id();
						std::string_view ptrTypeName(Utility::MetaType::typeName(typeId));

						UnloadRenderPassImpl(Utility::InternedString::InternedString(ptrTypeName));
					}
					static void Collect();
				public:
					RenderPassManager();
					~RenderPassManager()override;
					NO_COPY_MOVE(RenderPassManager)
				};
			}
		}
	}
}