#include "Texture2DLoader.hpp"
#include <fstream>
#include <opencv2/imgcodecs.hpp>
#include <vulkan/vk_enum_string_helper.h>
#include <vulkan/vulkan_format_traits.hpp>
#include "AirEngine/Runtime/Graphic/Manager/DeviceManager.hpp"
#include "AirEngine/Runtime/Utility/VulkanOpenCVTypeTransfer.hpp"
#include "AirEngine/Runtime/Utility/StringToVulkanypeTransfer.hpp"
#include "AirEngine/Runtime/Graphic/Instance/Buffer.hpp"
#include "AirEngine/Runtime/Graphic/Instance/Image.hpp"
#include "AirEngine/Runtime/Graphic/Command/CommandPool.hpp"
#include "AirEngine/Runtime/Graphic/Command/CommandBuffer.hpp"
#include "AirEngine/Runtime/Graphic/Command/Fence.hpp"
#include "AirEngine/Runtime/Graphic/Command/Barrier.hpp"
#include "AirEngine/Runtime/Graphic/Asset/Texture2D.hpp"
#include <boost/ref.hpp>
#include <opencv2/imgproc.hpp>
#include "AirEngine/Runtime/Core/Manager/TaskManager.hpp"

AirEngine::Runtime::Asset::AssetBase* AirEngine::Runtime::Graphic::Asset::Loader::Texture2DLoader::OnLoadAsset(const std::string& path, std::shared_future<void>& loadOperationFuture, bool& isInLoading)
{
	auto&& texture2d = new AirEngine::Runtime::Graphic::Asset::Texture2D();
	bool* isLoadingPtr = &isInLoading;

	loadOperationFuture = std::move(
		std::shared_future<void>(
			std::move(
				Core::Manager::TaskManager::Executor().async(
					[texture2d, path, isLoadingPtr]()->void
					{
						PopulateTexture2D(texture2d, path, isLoadingPtr);
					}
				)
			)
		)
	);
	return texture2d;
}

void AirEngine::Runtime::Graphic::Asset::Loader::Texture2DLoader::OnUnloadAsset(AirEngine::Runtime::Asset::AssetBase* asset)
{
	delete static_cast<AirEngine::Runtime::Graphic::Asset::Texture2D*>(asset);
}

void AirEngine::Runtime::Graphic::Asset::Loader::Texture2DLoader::PopulateTexture2D(AirEngine::Runtime::Graphic::Asset::Texture2D* texture2d, const std::string path, bool* isInLoading)
{
	//Load descriptor
	Descriptor descriptor{};
	{
		std::ifstream descriptorFile(path);
		if (!descriptorFile.is_open()) qFatal("Failed to open file.");
		nlohmann::json jsonFile = nlohmann::json::parse((std::istreambuf_iterator<char>(descriptorFile)), std::istreambuf_iterator<char>());
		descriptor = jsonFile.get<Descriptor>();
		descriptorFile.close();
	}

	//Parse descriptor data
	const vk::Format originalFormat = Utility::StringToVulkanypeTransfer::ParseToVkFormat(descriptor.originalFormat);
	const vk::Format targetFormat = Utility::StringToVulkanypeTransfer::ParseToVkFormat(descriptor.format);
	const bool topDown = descriptor.topDown;
	vk::ImageUsageFlags imageUsageFlags = {};
	vk::MemoryPropertyFlags memoryPropertyFlags = {};
	vk::ImageLayout imageLayout = Utility::StringToVulkanypeTransfer::ParseToVkImageLayout(descriptor.imageLayout);
	vk::ImageAspectFlags imageAspectFlags = {};
	uint32_t desiredMipmapLevelCount = 0;
	{
		for (const auto& imageUsageFlag : descriptor.imageUsageFlags)
		{
			imageUsageFlags = imageUsageFlags | Utility::StringToVulkanypeTransfer::ParseToVkImageUsageFlagBits(imageUsageFlag);
		}
		for (const auto& memoryPropertyFlag : descriptor.memoryPropertyFlags)
		{
			memoryPropertyFlags = memoryPropertyFlags | Utility::StringToVulkanypeTransfer::ParseToVkMemoryPropertyFlagBits(memoryPropertyFlag);
		}
		for (const auto& imageAspectFlag : descriptor.imageAspectFlags)
		{
			imageAspectFlags = imageAspectFlags | Utility::StringToVulkanypeTransfer::ParseToVkImageAspectFlagBits(imageAspectFlag);
		}
		{
			if (descriptor.mipmapGenerateType == "min")
			{
				desiredMipmapLevelCount = 1;
			}
			else if (descriptor.mipmapGenerateType == "auto")
			{
				desiredMipmapLevelCount = descriptor.perMipmapLevelTexturePath.size();
			}
			else if (descriptor.mipmapGenerateType == "max")
			{
				desiredMipmapLevelCount = std::numeric_limits<uint32_t>::max();
			}
			else
			{
				desiredMipmapLevelCount = std::atoi(descriptor.mipmapGenerateType.c_str());
			}
		}
	}

	if (descriptor.perMipmapLevelTexturePath.size() == 0) qFatal("No texture path provided.");

	//Load first texture for calculating meta data
	vk::Extent3D imageMaxExtent{};
	uint32_t maxMipmapLevelCount{};
	cv::Mat firstCvImage{};
	int cvImageChannelCount{};
	{
		{
			std::filesystem::path imagePath(descriptor.perMipmapLevelTexturePath[0]);
			if (!std::filesystem::exists(imagePath)) qFatal("Image do not exist.");
			firstCvImage = cv::imread(descriptor.perMipmapLevelTexturePath[0], cv::ImreadModes::IMREAD_ANYCOLOR | cv::ImreadModes::IMREAD_ANYDEPTH);
		}
		imageMaxExtent = VkExtent3D{ uint32_t(firstCvImage.cols), uint32_t(firstCvImage.rows), 1 };
		maxMipmapLevelCount = static_cast<uint32_t>(std::floor(std::log2(std::max(imageMaxExtent.width, imageMaxExtent.height)))) + 1;
		cvImageChannelCount = firstCvImage.channels();
	}

	//Calculate meta data
	uint8_t originalCvImageChannelCount{};
	int originalCvImageValueType{};
	uint32_t needLoadMipmapCount{};
	uint32_t needGenerateMipmapCount{};
	uint32_t mipmapLevelCount{};
	bool needSwapRBChannel{};
	bool needConvertType{};
	std::vector<vk::Extent3D> pmlImageExtent{};
	const auto isDirectCopy = originalFormat == targetFormat;
	{
		int originalCvImagePerChannelValueType = -1;
		{
			std::string errorCode{};
			originalCvImagePerChannelValueType = Utility::VulkanOpenCVTypeTransfer::VkFormatToPerChannelValueType(originalFormat, errorCode);
			if (originalCvImagePerChannelValueType == -1) qFatal(errorCode.c_str());
		}
		originalCvImageChannelCount = vk::componentCount(vk::Format(originalFormat));
		originalCvImageValueType = CV_MAKETYPE(originalCvImagePerChannelValueType, originalCvImageChannelCount);

		mipmapLevelCount = desiredMipmapLevelCount == std::numeric_limits<uint32_t>::max() ? maxMipmapLevelCount : desiredMipmapLevelCount;
		mipmapLevelCount = std::min(mipmapLevelCount, maxMipmapLevelCount);

		needLoadMipmapCount = std::min(mipmapLevelCount, uint32_t(descriptor.perMipmapLevelTexturePath.size()));
		needGenerateMipmapCount = mipmapLevelCount - needLoadMipmapCount;

		needSwapRBChannel = cvImageChannelCount >= 3 && std::strcmp(vk::componentName(vk::Format(originalFormat), 0), "R") == 0;
		needConvertType = firstCvImage.type() != originalCvImageValueType;

		pmlImageExtent = std::move(Graphic::Instance::Image::GetPerMipmapLevelExtent3D(imageMaxExtent, mipmapLevelCount));
	}

	std::vector<cv::Mat> pmlOriginalCvImage(mipmapLevelCount);
	std::vector<size_t> pmlByteOffset(mipmapLevelCount, 0);
	std::vector<size_t> pmlByteSize(mipmapLevelCount, 0);
	size_t totalByteSize = 0;
	{
		int mipMapLevelIndex = 0;

		auto cvImage = std::move(firstCvImage);
		std::vector<cv::Mat> channels{};

		for (; mipMapLevelIndex < needLoadMipmapCount; mipMapLevelIndex++)
		{
			auto& originalCvImage = pmlOriginalCvImage[mipMapLevelIndex];

			if (mipMapLevelIndex > 0)
			{
				std::filesystem::path imagePath(descriptor.perMipmapLevelTexturePath[0]);
				if (!std::filesystem::exists(imagePath)) qFatal("Image do not exist.");
				cvImage = cv::imread(descriptor.perMipmapLevelTexturePath[0], cv::ImreadModes::IMREAD_ANYCOLOR | cv::ImreadModes::IMREAD_ANYDEPTH);
			}

			//Trim cv image channel count
			{
				bool isSplited = false;
				if (needSwapRBChannel)
				{
					if (!isSplited)
					{
						channels.resize(cvImageChannelCount);
						cv::split(cvImage, channels);
						isSplited = true;
					}
					std::swap(channels[0], channels[2]);
				}
				if (originalCvImageChannelCount != cvImageChannelCount)
				{
					if (!isSplited)
					{
						channels.resize(cvImageChannelCount);
						cv::split(cvImage, channels);
						isSplited = true;
					}
					channels.resize(originalCvImageChannelCount, cv::Mat(channels[0].rows, channels[0].cols, channels[0].type()).setTo(1));
					cv::merge(channels, cvImage);
				}
			}

			//Trim cv image type
			{
				if (needConvertType)
				{
					cvImage.convertTo(originalCvImage, originalCvImageValueType);
				}
				else
				{
					originalCvImage = std::move(cvImage);
				}
			}

			//Trim cv image continuous
			if (!originalCvImage.isContinuous())
			{
				originalCvImage = originalCvImage.clone();
			}

			pmlByteOffset[mipMapLevelIndex] = totalByteSize;
			pmlByteSize[mipMapLevelIndex] = originalCvImage.total() * originalCvImage.elemSize();
			totalByteSize += pmlByteSize[mipMapLevelIndex];
		}

		for (; mipMapLevelIndex < mipmapLevelCount; mipMapLevelIndex++)
		{
			auto& preMlCvImage = pmlOriginalCvImage[mipMapLevelIndex - 1];
			auto& originalCvImage = pmlOriginalCvImage[mipMapLevelIndex];
			cv::resize(
				preMlCvImage, 
				originalCvImage,
				cv::Size(pmlImageExtent[mipMapLevelIndex].width, pmlImageExtent[mipMapLevelIndex].height), 
				0.0, 0.0, 
				cv::InterpolationFlags::INTER_LINEAR
			);

			//Trim cv image continuous
			if (!originalCvImage.isContinuous())
			{
				originalCvImage = originalCvImage.clone();
			}

			pmlByteOffset[mipMapLevelIndex] = totalByteSize;
			pmlByteSize[mipMapLevelIndex] = originalCvImage.total() * originalCvImage.elemSize();
			totalByteSize += pmlByteSize[mipMapLevelIndex];
		}
	}

	auto&& stagingBuffer = Graphic::Instance::Buffer(
		totalByteSize,
		vk::BufferUsageFlagBits::eTransferSrc,
		vk::MemoryPropertyFlagBits::eHostVisible | vk::MemoryPropertyFlagBits::eHostCoherent,
		VMA_ALLOCATION_CREATE_HOST_ACCESS_SEQUENTIAL_WRITE_BIT
	);
	auto data = stagingBuffer.Memory()->Map();
	for (uint32_t mipMapLevelIndex = 0; mipMapLevelIndex < mipmapLevelCount; mipMapLevelIndex++)
	{
		std::memcpy(
			static_cast<uint8_t*>(data) + pmlByteOffset[mipMapLevelIndex], 
			pmlOriginalCvImage[mipMapLevelIndex].data,
			pmlByteSize[mipMapLevelIndex]
		);
	}
	stagingBuffer.Memory()->Unmap();

	auto&& commandPool = Graphic::Command::CommandPool(Utility::InternedString("TransferQueue"), vk::CommandPoolCreateFlagBits::eTransient);
	auto&& commandBuffer = commandPool.CreateCommandBuffer(Utility::InternedString("TransferCommandBuffer"));
	Graphic::Command::Barrier barrier{};
	auto&& transferFence = Graphic::Command::Fence(false);

	auto&& targetImage = texture2d->PopulateDataAndCreateInstance(
		targetFormat,
		imageMaxExtent,
		vk::ImageType::e2D,
		1, mipmapLevelCount,
		vk::ImageUsageFlagBits::eTransferDst | imageUsageFlags,
		memoryPropertyFlags
	);
	for (const auto& viewDescriptor : descriptor.views)
	{
		targetImage->AddImageView(Utility::InternedString(viewDescriptor.name), vk::ImageViewType::e2D, Utility::StringToVulkanypeTransfer::ParseToVkImageLayout(viewDescriptor.layout), vk::ImageAspectFlagBits::eColor, viewDescriptor.baseMipmapLevel, viewDescriptor.mipmapLevelCount, 0, 1);
	}
	auto&& originalImage = std::unique_ptr<Graphic::Instance::Image>(isDirectCopy ? nullptr : new Graphic::Instance::Image(
		originalFormat,
		imageMaxExtent,
		vk::ImageType::e2D,
		1, mipmapLevelCount,
		vk::ImageUsageFlagBits::eTransferSrc | vk::ImageUsageFlagBits::eTransferDst,
		vk::MemoryPropertyFlagBits::eDeviceLocal
	));

	commandBuffer.BeginRecord(vk::CommandBufferUsageFlagBits::eOneTimeSubmit);
	if (isDirectCopy)
	{
		{
			barrier.AddImageMemoryBarrier(
				*targetImage,
				vk::PipelineStageFlagBits2::eNone,
				vk::AccessFlagBits2::eNone,
				vk::PipelineStageFlagBits2::eTransfer,
				vk::AccessFlagBits2::eTransferWrite,
				vk::ImageLayout::eUndefined,
				vk::ImageLayout::eTransferDstOptimal,
				imageAspectFlags
			);
		}
		commandBuffer.AddPipelineBarrier(barrier);
		commandBuffer.CopyBufferToImage(stagingBuffer, *targetImage, vk::ImageLayout::eTransferDstOptimal, imageAspectFlags);
		{
			barrier.ClearImageMemoryBarriers();
			barrier.AddImageMemoryBarrier(
				*targetImage,
				vk::PipelineStageFlagBits2::eTransfer,
				vk::AccessFlagBits2::eTransferWrite,
				vk::PipelineStageFlagBits2::eNone,
				vk::AccessFlagBits2::eNone,
				vk::ImageLayout::eTransferDstOptimal,
				imageLayout,
				imageAspectFlags
			);
		}
		commandBuffer.AddPipelineBarrier(barrier);
	}
	else
	{
		{
			barrier.AddImageMemoryBarrier(
				*originalImage,
				vk::PipelineStageFlagBits2::eNone,
				vk::AccessFlagBits2::eNone,
				vk::PipelineStageFlagBits2::eTransfer,
				vk::AccessFlagBits2::eTransferWrite,
				vk::ImageLayout::eUndefined,
				vk::ImageLayout::eTransferDstOptimal,
				imageAspectFlags
			);
		}
		commandBuffer.AddPipelineBarrier(barrier);
		commandBuffer.CopyBufferToImage(stagingBuffer, *originalImage, vk::ImageLayout::eTransferDstOptimal, imageAspectFlags);
		{
			barrier.ClearImageMemoryBarriers();
			barrier.AddImageMemoryBarrier(
				*originalImage,
				vk::PipelineStageFlagBits2::eTransfer,
				vk::AccessFlagBits2::eTransferWrite,
				vk::PipelineStageFlagBits2::eBlit,
				vk::AccessFlagBits2::eTransferRead,
				vk::ImageLayout::eTransferDstOptimal,
				vk::ImageLayout::eTransferSrcOptimal,
				imageAspectFlags
			);
			barrier.AddImageMemoryBarrier(
				*targetImage,
				vk::PipelineStageFlagBits2::eTransfer,
				vk::AccessFlagBits2::eNone,
				vk::PipelineStageFlagBits2::eBlit,
				vk::AccessFlagBits2::eTransferWrite,
				vk::ImageLayout::eUndefined,
				vk::ImageLayout::eTransferDstOptimal,
				imageAspectFlags
			);
		}
		commandBuffer.AddPipelineBarrier(barrier);
		commandBuffer.Blit(
			*originalImage, vk::ImageLayout::eTransferSrcOptimal,
			*targetImage, vk::ImageLayout::eTransferDstOptimal,
			imageAspectFlags, vk::Filter::eLinear
		);
		{
			barrier.ClearImageMemoryBarriers();
			barrier.AddImageMemoryBarrier(
				*targetImage,
				vk::PipelineStageFlagBits2::eBlit,
				vk::AccessFlagBits2::eTransferWrite,
				vk::PipelineStageFlagBits2::eTransfer,
				vk::AccessFlagBits2::eNone,
				vk::ImageLayout::eTransferDstOptimal,
				imageLayout,
				imageAspectFlags
			);
		}
		commandBuffer.AddPipelineBarrier(barrier);
	}

	commandBuffer.EndRecord();

	commandPool.Queue().ImmediateIndividualSubmit(
		{ {}, { &commandBuffer }, {} },
		transferFence
	);

	while (transferFence.Status() == vk::Result::eNotReady) std::this_thread::yield();

	*isInLoading = false;
}

AirEngine::Runtime::Graphic::Asset::Loader::Texture2DLoader::Texture2DLoader()
	: LoaderBase("Texture2DLoader", "texture2d")
{

}
AirEngine::Runtime::Graphic::Asset::Loader::Texture2DLoader::~Texture2DLoader()
{
}
