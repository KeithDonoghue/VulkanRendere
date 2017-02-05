#include "VulkanImage.h"
#include "VulkanDevice.h"

#include "CommandPool.h"
#include "CommandBuffer.h"
#include "VulkanBuffer.h"

#include "vulkan/vulkan.h"

#define ENGINE_LOGGING_ENABLED 1 
#include "EngineLogging.h"


#include "ImageLoader.h"

#include <algorithm>





VulkanImage::VulkanImage(VulkanDevice * theDevice, VkImage theImage, uint32_t width, uint32_t height, bool systemManaged) :
mDevice(theDevice),
m_TheVulkanImage(theImage),
mSystemManaged(systemManaged),
mType(VULKAN_IMAGE_COLOR_RGBA8),
mWidth(width),
mHeight(height)
{}





VulkanImage::VulkanImage(VulkanDevice * theDevice, int width, int height, ImageType type):
mCreateInfo{},
mExtent{},
mWidth(width),
mHeight(height),
mDevice(theDevice),
mType(type)
{
	Init();
}





VulkanImage::VulkanImage(VulkanDevice * theDevice, std::string filename):
mDevice(theDevice),
mType(ImageType::VULKAN_IMAGE_COLOR_RGBA8)
{
	int x, y, n;
	unsigned char * data = stbi_load(filename.c_str(), &x, &y, &n, 4);

	mWidth = x;
	mHeight = y;

	Init();

	ClearImage(1.0f);

	mStagingBuffer = VulkanBuffer::CreateStagingBuffer(*mDevice, x*y * 4);
	mStagingBuffer->LoadBufferData(data, x*y * 4);

	stbi_image_free(data);

	LoadDataToImage();
}





void VulkanImage::Init()
{
	mExtent = { mWidth, mHeight, 1 };
	mCreateInfo.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
	mCreateInfo.pNext = nullptr;
	mCreateInfo.flags = 0;
	mCreateInfo.imageType = VK_IMAGE_TYPE_2D;
	mCreateInfo.format = GetFormat();
	mCreateInfo.extent = mExtent;
	mCreateInfo.mipLevels = 1;
	mCreateInfo.arrayLayers = 1;
	mCreateInfo.samples = VK_SAMPLE_COUNT_1_BIT;
	mCreateInfo.tiling = VK_IMAGE_TILING_OPTIMAL;
	mCreateInfo.usage = VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_TRANSFER_SRC_BIT | GetUsageForType(mType);
	mCreateInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
	mCreateInfo.queueFamilyIndexCount = 0;
	mCreateInfo.pQueueFamilyIndices = nullptr;
	mCreateInfo.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;

	VkResult result = vkCreateImage(mDevice->getVkDevice(), &mCreateInfo, nullptr, &m_TheVulkanImage);

	if (result != VK_SUCCESS)
	{
		EngineLog("Couldn't create VulkanImage");
	}


	mCurrentLayout = mCreateInfo.initialLayout;

	VkMemoryRequirements memoryRequirements = {};
	vkGetImageMemoryRequirements(mDevice->getVkDevice(), m_TheVulkanImage, &memoryRequirements);

	mAllocStruct = mDevice->GetMemManager().GetAllocation(memoryRequirements);

	result = vkBindImageMemory(mDevice->getVkDevice(), m_TheVulkanImage, mAllocStruct.mPointer, mAllocStruct.mOffset);

	if (result != VK_SUCCESS)
	{
		EngineLog("Couldn't bind image memory.");
	}
}





VulkanImage::~VulkanImage()
{
	if (!mSystemManaged)
	{
		vkQueueWaitIdle(mDevice->GetVkQueue());
		mDevice->GetMemManager().FreeAllocation(mAllocStruct);
		vkDestroyImage(mDevice->getVkDevice(), m_TheVulkanImage, nullptr);
	}
}





void VulkanImage::ClearImage(float value)
{
	VkImageSubresourceRange subRange = {};

	subRange.baseMipLevel = 0;
	subRange.levelCount = 1;
	subRange.baseArrayLayer = 0;
	subRange.layerCount = 1;


	// May need Image Layouot conversion, memory barrier or queue family transfer.
	if (mType == ImageType::VULKAN_IMAGE_COLOR_RGBA8)
	{
		ClearColourImage(subRange, value);
	}
	else if (mType == ImageType::VULKAN_IMAGE_DEPTH)
	{
		ClearDepthImage(subRange, value);
	}
	else
	{
		EngineLog("Error, unknown ImageType");
	}
}





void VulkanImage::ClearColourImage(VkImageSubresourceRange& subRange, float value)
{

	subRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;

	TransitionToClearable(subRange);


	VkClearColorValue clearColour;
	memset(&clearColour, 0, sizeof(VkClearColorValue));
	clearColour.float32[0] = value;
	clearColour.float32[1] = value;
	clearColour.float32[2] = value;
	clearColour.float32[3] = 0.0f;

	CommandBuffer * currentCommandBuffer = mDevice->GetCommandPool().GetCurrentCommandBuffer();

	vkCmdClearColorImage(currentCommandBuffer->GetVkCommandBuffer(), GetVkImage(), GetLayout(), &clearColour, 1, &subRange);

}





void VulkanImage::ClearDepthImage(VkImageSubresourceRange& subRange, float value)
{
	subRange.aspectMask = VK_IMAGE_ASPECT_DEPTH_BIT;

	TransitionToClearable(subRange);


	VkClearDepthStencilValue clearValue = {};
	clearValue.depth = value;
	
	CommandBuffer * currentCommandBuffer = mDevice->GetCommandPool().GetCurrentCommandBuffer();

	vkCmdClearDepthStencilImage(currentCommandBuffer->GetVkCommandBuffer(), GetVkImage(), GetLayout(), &clearValue, 1, &subRange);
}





void VulkanImage::InsertImageMemoryBarrier(VkImageSubresourceRange& subRange, VkImageLayout newLayout)
{
	if (newLayout == GetLayout())
		return;

	CommandBuffer * currentCommandBuffer = mDevice->GetCommandPool().GetCurrentCommandBuffer();
	currentCommandBuffer->BeginCommandBuffer();


	//Currently only 1 clear value.

	VkAccessFlags dstFlags = GetAccessFlags(newLayout);
	VkAccessFlags srcFlags = GetAccessFlags(GetLayout());

	VkImageMemoryBarrier memoryBarrier = {};

	memoryBarrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
	memoryBarrier.pNext = nullptr;
	memoryBarrier.srcAccessMask = srcFlags;
	memoryBarrier.dstAccessMask = dstFlags;
	memoryBarrier.oldLayout = GetLayout();
	memoryBarrier.newLayout = newLayout;
	memoryBarrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
	memoryBarrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
	memoryBarrier.image = GetVkImage();
	memoryBarrier.subresourceRange = subRange;

	SetLayout(newLayout);

	vkCmdPipelineBarrier(currentCommandBuffer->GetVkCommandBuffer(), VK_PIPELINE_STAGE_ALL_COMMANDS_BIT, VK_PIPELINE_STAGE_BOTTOM_OF_PIPE_BIT,
		0, 0, nullptr, 0, nullptr, 1, &memoryBarrier);
}





void VulkanImage::CopyImageData(VulkanImage& src)
{

	CommandBuffer * currentCommandBuffer = mDevice->GetCommandPool().GetCurrentCommandBuffer();

	currentCommandBuffer->BeginCommandBuffer();

	VkImageCopy copyRegion = {};

	VkImageSubresourceLayers srcSubresource = {};
	VkImageSubresourceLayers dstSubresource = {};


	VkImageSubresourceRange barrierSubRange = {};

	barrierSubRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
	barrierSubRange.baseArrayLayer = 0;
	barrierSubRange.baseMipLevel = 0;
	barrierSubRange.layerCount = 1;
	barrierSubRange.levelCount = 1;


	src.InsertImageMemoryBarrier(barrierSubRange, VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL);

	InsertImageMemoryBarrier(barrierSubRange, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL);

	srcSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
	dstSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;

	srcSubresource.baseArrayLayer = 0;
	dstSubresource.baseArrayLayer = 0;

	srcSubresource.layerCount = 1;
	dstSubresource.layerCount = 1;

	srcSubresource.mipLevel = 0;
	dstSubresource.mipLevel = 0;

	copyRegion.srcSubresource = srcSubresource;
	copyRegion.srcOffset = { 0, 0, 0 };
	copyRegion.dstSubresource = dstSubresource;
	copyRegion.dstOffset = { 0, 0, 0 };

	copyRegion.extent = { std::min(mWidth, src.mWidth), std::min(mHeight, src.mHeight), 1 };

	vkCmdCopyImage(currentCommandBuffer->GetVkCommandBuffer(),
		src.GetVkImage(),
		VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL,
		GetVkImage(),
		VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
		1,
		&copyRegion);
}





void VulkanImage::LoadDataToImage()
{

	VkImageSubresourceLayers subLayers = {};
	subLayers.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
	subLayers.baseArrayLayer = 0;
	subLayers.layerCount = 1;
	subLayers.mipLevel = 0;

	VkBufferImageCopy copyInfo = {};
	copyInfo.bufferOffset = 0;
	copyInfo.bufferRowLength = 500;
	copyInfo.bufferImageHeight = 0;
	copyInfo.imageSubresource = subLayers;
	copyInfo.imageOffset = { 0, 0, 0 };
	copyInfo.imageExtent = { mWidth, mHeight, 1};
	

	VkImageSubresourceRange barrierSubRange = {};

	barrierSubRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
	barrierSubRange.baseArrayLayer = 0;
	barrierSubRange.baseMipLevel = 0;
	barrierSubRange.layerCount = 1;
	barrierSubRange.levelCount = 1;


	CommandBuffer * currentCommandBuffer = mDevice->GetCommandPool().GetCurrentCommandBuffer();
	currentCommandBuffer->BeginCommandBuffer();

	InsertImageMemoryBarrier(barrierSubRange, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL);

	vkCmdCopyBufferToImage(currentCommandBuffer->GetVkCommandBuffer(), 
		mStagingBuffer->GetVkBuffer(), 
		GetVkImage(), 
		GetLayout(), 
		1, 
		&copyInfo);
}





void VulkanImage::BlitFullImage(VulkanImage& src)
{
	CommandBuffer * currentCommandBuffer = mDevice->GetCommandPool().GetCurrentCommandBuffer();
	currentCommandBuffer->BeginCommandBuffer();

	VkImageBlit blitRegions = {};
	VkImageSubresourceLayers subresources = {};
	subresources.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
	subresources.baseArrayLayer = 0;
	subresources.layerCount = 1;
	subresources.mipLevel = 0;



	blitRegions.srcSubresource = subresources;
	blitRegions.srcOffsets[0] = { 0, 0, 0 };
	blitRegions.srcOffsets[1] = { src.getWidth(), src.getHeight(), 1 };

	blitRegions.dstSubresource = subresources;
	blitRegions.dstOffsets[0] = { 0, 0, 0 };
	blitRegions.dstOffsets[1] = { getWidth(), getHeight(), 1 };

	src.BlittingFrom();
	TransToRecieveBlit();
	static int doBlit = 0;


	vkCmdBlitImage(currentCommandBuffer->GetVkCommandBuffer(), 
		src.getVkImage(), 
		VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL, 
		getVkImage(),
		VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
		1,
		&blitRegions,
		VK_FILTER_LINEAR);
}




VkFormat VulkanImage::GetFormatForType(ImageType type)
{
	static VkFormat FormatMap[ImageType::VULKAN_IMAGE_RANGE_SIZE] =
	{
		VK_FORMAT_R8G8B8A8_UNORM,
		VK_FORMAT_D16_UNORM
	};

	return FormatMap[type];
}





VkFormat VulkanImage::GetFormat()
{
	return GetFormatForType(mType);
}





VkImageUsageFlags VulkanImage::GetUsageForType(ImageType type)
{
	static VkImageUsageFlags UsageBitsMap[ImageType::VULKAN_IMAGE_RANGE_SIZE] =
	{
		VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT |
		VK_IMAGE_USAGE_SAMPLED_BIT,
		VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT
	};

	return UsageBitsMap[type];
}





void VulkanImage::TransitionToClearable(VkImageSubresourceRange& subRange)
{
	if (GetLayout() == VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL ||
		GetLayout() == VK_IMAGE_LAYOUT_GENERAL)
	{
		return;
	}

	InsertImageMemoryBarrier(subRange, VK_IMAGE_LAYOUT_GENERAL);
}




VkImageAspectFlags VulkanImage::getAspectFlags()
{
	static VkImageAspectFlags accessMap[ImageType::VULKAN_IMAGE_RANGE_SIZE] =
	{
		VK_IMAGE_ASPECT_COLOR_BIT,
		VK_IMAGE_ASPECT_DEPTH_BIT
	};

	return accessMap[getType()];
}





void VulkanImage::TransToRecieveBlit()
{
	VkImageSubresourceRange subRange = {};
	subRange.aspectMask = getAspectFlags();
	subRange.baseArrayLayer = 0;
	subRange.baseMipLevel = 0;
	subRange.layerCount = 1;
	subRange.levelCount = 1;

	InsertImageMemoryBarrier(subRange, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL);
}





void VulkanImage::BlittingFrom()
{
	VkImageSubresourceRange subRange = {};
	subRange.aspectMask = getAspectFlags();
	subRange.baseArrayLayer = 0;
	subRange.baseMipLevel = 0;
	subRange.layerCount = 1;
	subRange.levelCount = 1;

	InsertImageMemoryBarrier(subRange, VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL);
}




VkAccessFlags VulkanImage::GetAccessFlags(VkImageLayout layout)
{
	if (layout == VK_IMAGE_LAYOUT_PRESENT_SRC_KHR)
		return VK_ACCESS_MEMORY_READ_BIT;

	static VkAccessFlags accessMap[VkImageLayout::VK_IMAGE_LAYOUT_RANGE_SIZE] =
	{
		0,
		0,
		VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT |
		VK_ACCESS_COLOR_ATTACHMENT_READ_BIT,
		VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT |
		VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_READ_BIT,
		0,
		0,
		VK_ACCESS_TRANSFER_READ_BIT,
		VK_ACCESS_TRANSFER_WRITE_BIT,
		0
	};

	return accessMap[layout];
}
