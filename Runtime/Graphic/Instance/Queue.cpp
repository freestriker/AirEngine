﻿#include "Queue.hpp"
#include "AirEngine/Runtime/Graphic/Command/Semaphore.hpp"
#include "AirEngine/Runtime/Graphic/Command/Fence.hpp"
#include "AirEngine/Runtime/Graphic/Command/CommandBuffer.hpp"
#include <qlogging.h>

//constexpr VkSemaphoreSubmitInfo EMPTY_VK_SEMAPHORE_SUBMIT_INFO
//{
//	VK_STRUCTURE_TYPE_SEMAPHORE_SUBMIT_INFO,
//	nullptr,
//	VK_NULL_HANDLE,
//	0,
//	VK_PIPELINE_STAGE_2_NONE,
//	0
//};
//
//constexpr VkCommandBufferSubmitInfo EMPTY_VK_COMMAND_BUFFER_SUBMIT_INFO
//{
//	VK_STRUCTURE_TYPE_COMMAND_BUFFER_SUBMIT_INFO,
//	nullptr,
//	VK_NULL_HANDLE,
//	0
//};
//
//constexpr VkSubmitInfo2 EMPTY_VK_SUBMIT_INFO
//{
//	VK_STRUCTURE_TYPE_SUBMIT_INFO_2,
//	nullptr,
//	0,
//	0,
//	nullptr,
//	0,
//	nullptr,
//	0,
//	nullptr
//};

void AirEngine::Runtime::Graphic::Instance::Queue::Submit(const std::vector<CommandBufferSubmitInfo>& submitInfos, const Command::Fence* fence)
{
	uint32_t semaphoreInfoCount = 0;
	uint32_t commandBufferInfoCount = 0;
	for (const auto& submitInfo : submitInfos)
	{
		semaphoreInfoCount += submitInfo.waitInfos.size();
		commandBufferInfoCount += submitInfo.commandBuffers.size();
		semaphoreInfoCount += submitInfo.signalInfos.size();
	}

	std::vector<vk::SemaphoreSubmitInfo> vkSemaphoreInfos(semaphoreInfoCount);
	std::vector<vk::CommandBufferSubmitInfo> vkCommandBufferInfos(commandBufferInfoCount);
	std::vector<vk::SubmitInfo2> vkSubmitInfos(submitInfos.size());
	uint32_t semaphoreInfoIndex = 0;
	uint32_t commandBufferInfoIndex = 0;
	uint32_t submitInfoIndex = 0;
	for (const auto& submitInfo : submitInfos)
	{
		auto&& vkSubmitInfo = vkSubmitInfos[submitInfoIndex++];
		vkSubmitInfo.waitSemaphoreInfoCount = uint32_t(submitInfo.waitInfos.size());
		vkSubmitInfo.pWaitSemaphoreInfos = vkSemaphoreInfos.data() + semaphoreInfoIndex;
		vkSubmitInfo.commandBufferInfoCount = uint32_t(vkCommandBufferInfos.size());
		vkSubmitInfo.pCommandBufferInfos = vkCommandBufferInfos.data() + commandBufferInfoIndex;
		vkSubmitInfo.signalSemaphoreInfoCount = uint32_t(submitInfo.signalInfos.size());
		vkSubmitInfo.pSignalSemaphoreInfos = vkSemaphoreInfos.data() + semaphoreInfoIndex + submitInfo.waitInfos.size();

		for (const auto& info : submitInfo.waitInfos)
		{
			auto&& vkSsemaphoreInfo = vkSemaphoreInfos[semaphoreInfoIndex++];
			vkSsemaphoreInfo.semaphore = info.semphore->VkHandle();
			vkSsemaphoreInfo.stageMask = info.stageMask;
		}
		for (const auto& info : submitInfo.commandBuffers)
		{
			auto&& vkCommandBufferInfo = vkCommandBufferInfos[commandBufferInfoIndex++];
			vkCommandBufferInfo.commandBuffer = info->VkHandle();
		}
		for (const auto& info : submitInfo.signalInfos)
		{
			auto&& vkSemaphoreInfo = vkSemaphoreInfos[semaphoreInfoIndex++];
			vkSemaphoreInfo.semaphore = info.semphore->VkHandle();
			vkSemaphoreInfo.stageMask = info.stageMask;
		}
	}

	_vkQueue.submit2(vkSubmitInfos, fence != nullptr ? fence->VkHandle() : vk::Fence());
}