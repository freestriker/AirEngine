﻿#include "RenderPassManager.hpp"
#include "../Instance/RenderPassBase.hpp"

AirEngine::Runtime::Utility::Fiber::mutex AirEngine::Runtime::Graphic::Manager::RenderPassManager::_managerMutex{};
std::unordered_map< AirEngine::Runtime::Utility::InternedString, AirEngine::Runtime::Graphic::Manager::RenderPassManager::ReferenceInfo> AirEngine::Runtime::Graphic::Manager::RenderPassManager::_referenceMap{};

AirEngine::Runtime::Graphic::Instance::RenderPassBase* AirEngine::Runtime::Graphic::Manager::RenderPassManager::LoadRenderPassImpl(const Utility::InternedString renderPassTypeName, int typeId)
{
    std::unique_lock<Utility::Fiber::mutex> locker(_managerMutex);

    auto&& iterator = _referenceMap.find(renderPassTypeName);
    if (iterator == std::end(_referenceMap))
    {
        if (typeId == 0)
        {
            qFatal(std::string("Do not have render pass type named: " + renderPassTypeName.ToString() + ".").c_str());
        }

        const Utility::MetaObject* metaObj = QMetaType::metaObjectForType(typeId);
        if (metaObj == nullptr)
        {
            qFatal(std::string("Do not have render pass type named: " + renderPassTypeName.ToString() + ".").c_str());
        }

        Utility::ReflectableObject* obj = metaObj->newInstance();
        if (obj == nullptr)
        {
            qFatal(std::string("Can not create render pass variant named: " + renderPassTypeName.ToString() + ".").c_str());
        }

        auto&& renderPassBase = qobject_cast<Instance::RenderPassBase*>(obj);
        _referenceMap.emplace(renderPassTypeName, ReferenceInfo{ 1, renderPassTypeName, renderPassBase });

        return renderPassBase;
    }
    else
    {
        iterator->second.refrenceCount++;

        return iterator->second.renderPass;
    }
}

void AirEngine::Runtime::Graphic::Manager::RenderPassManager::UnloadRenderPassImpl(const Utility::InternedString renderPassTypeName)
{
    std::unique_lock<Utility::Fiber::mutex> locker(_managerMutex);

    auto&& iterator = _referenceMap.find(renderPassTypeName);
    if (iterator == std::end(_referenceMap))
    {
        qFatal(std::string("Do not have render pass instance named: " + renderPassTypeName.ToString() + ".").c_str());
    }

    if (iterator->second.refrenceCount == 0)
    {
        qFatal(std::string("Unload render pass too many times named: " + renderPassTypeName.ToString() + ".").c_str());
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