#include "RenderPassManager.hpp"
#include <rttr/type.h>

AirEngine::Runtime::Utility::Fiber::mutex AirEngine::Runtime::Graphic::Manager::RenderPassManager::_managerMutex{};
std::unordered_map< AirEngine::Runtime::Utility::InternedString, AirEngine::Runtime::Graphic::Manager::RenderPassManager::ReferenceInfo> AirEngine::Runtime::Graphic::Manager::RenderPassManager::_referenceMap{};

AirEngine::Runtime::Graphic::Instance::RenderPassBase* AirEngine::Runtime::Graphic::Manager::RenderPassManager::LoadRenderPass(const std::string& renderPassTypeName)
{
    std::unique_lock<Utility::Fiber::mutex> locker(_managerMutex);

    auto&& renderPassTypeNameInternedString = Utility::InternedString::InternedString(renderPassTypeName);

    auto&& iterator = _referenceMap.find(renderPassTypeNameInternedString);
    if (iterator == std::end(_referenceMap))
    {
        rttr::type renderPassType = rttr::type::get_by_name(renderPassTypeName);
        if (!renderPassType.is_valid())
        {
            qFatal(std::string("Do not have render pass type named: " + renderPassTypeName + ".").c_str());
        }

        rttr::variant renderPassVariant = renderPassType.create();
        if (!renderPassVariant.is_valid())
        {
            qFatal(std::string("Can not create render pass variant named: " + renderPassTypeName + ".").c_str());
        }

        AirEngine::Runtime::Graphic::Instance::RenderPassBase* renderPass = renderPassVariant.get_value<AirEngine::Runtime::Graphic::Instance::RenderPassBase*>();
        if (renderPass == nullptr)
        {
            qFatal(std::string("Can not cast render pass named: " + renderPassTypeName + ".").c_str());
        }

        _referenceMap.emplace(renderPassTypeNameInternedString, ReferenceInfo{ 1, renderPassTypeNameInternedString, renderPass });
        
        return renderPass;
    }
    else
    {
        iterator->second.refrenceCount++;

        return iterator->second.renderPass;
    }
}

void AirEngine::Runtime::Graphic::Manager::RenderPassManager::UnloadRenderPass(const std::string& renderPassTypeName)
{
    std::unique_lock<Utility::Fiber::mutex> locker(_managerMutex);

    auto&& renderPassTypeNameInternedString = Utility::InternedString::InternedString(renderPassTypeName);

    auto&& iterator = _referenceMap.find(renderPassTypeNameInternedString);
    if (iterator == std::end(_referenceMap))
    {
        qFatal(std::string("Do not have render pass instance named: " + renderPassTypeName + ".").c_str());
    }

    if (iterator->second.refrenceCount == 0)
    {
        qFatal(std::string("Unload render pass too many times named: " + renderPassTypeName + ".").c_str());
    }

    iterator->second.refrenceCount--;
}

void AirEngine::Runtime::Graphic::Manager::RenderPassManager::Collect()
{
    std::unique_lock<Utility::Fiber::mutex> locker(_managerMutex);

    for (auto iterator = _referenceMap.begin(); iterator != _referenceMap.end(); )
    {
        if (iterator->second.refrenceCount == 0)
        {
            delete iterator->second.renderPass;
            iterator = _referenceMap.erase(iterator);
        }
        else
        {
            ++iterator;
        }
    }
}
