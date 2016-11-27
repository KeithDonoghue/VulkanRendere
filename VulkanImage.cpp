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

VulkanImage::VulkanImage(VulkanDevice * theDevice, VkImage theImage, bool systemManaged) :
mDevice(theDevice),
m_TheVulkanImage(theImage),
mSystemManaged(systemManaged)
{}





VulkanImage::VulkanImage(VulkanDevice * theDevice, int width, int height):
mCreateInfo{},
mExtent{},
mDevice(theDevice)
{
	mExtent = { width, height, 1 };
	mCreateInfo.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
	mCreateInfo.pNext = nullptr;
	mCreateInfo.flags = 0;
	mCreateInfo.imageType = VK_IMAGE_TYPE_2D;
	mCreateInfo.format = VK_FORMAT_B8G8R8A8_UNORM;
	mCreateInfo.extent = mExtent;
	mCreateInfo.mipLevels = 1;
	mCreateInfo.arrayLayers = 1;
	mCreateInfo.samples = VK_SAMPLE_COUNT_1_BIT;
	mCreateInfo.tiling = VK_IMAGE_TILING_OPTIMAL;
	mCreateInfo.usage = VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_TRANSFER_SRC_BIT;
	mCreateInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
	mCreateInfo.queueFamilyIndexCount = 0;
	mCreateInfo.pQueueFamilyIndices = nullptr;
	mCreateInfo.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;

	VkResult result = vkCreateImage(mDevice->GetVkDevice(), &mCreateInfo, nullptr, &m_TheVulkanImage);

	if (result != VK_SUCCESS)
	{
		EngineLog("Couldn't create VulkanImage");
	}


	mCurrentLayout = mCreateInfo.initialLayout;

	VkMemoryRequirements memoryRequirements = {};
	vkGetImageMemoryRequirements(mDevice->GetVkDevice(), m_TheVulkanImage, &memoryRequirements);

	mAllocStruct = mDevice->GetMemManager()->GetAllocation(memoryRequirements);

	result = vkBindImageMemory(mDevice->GetVkDevice(), m_TheVulkanImage, mAllocStruct.mPointer, mAllocStruct.mOffset);

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
		delete mStagingBuffer;
		mDevice->GetMemManager()->FreeAllocation(mAllocStruct);
		vkDestroyImage(mDevice->GetVkDevice(), m_TheVulkanImage, nullptr);
	}
}





void VulkanImage::ClearImage(float green)
{

	VkImageSubresourceRange subRange;
	memset(&subRange, 0, sizeof(VkImageSubresourceRange));

	subRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
	subRange.baseMipLevel = 0;
	subRange.levelCount = 1;
	subRange.baseArrayLayer = 0;
	subRange.layerCount = 1;

	// May need Image Layouot conversion, memory barrier or queue family transfer.
	InsertImageMemoryBarrier(subRange, VK_IMAGE_LAYOUT_GENERAL);


	VkClearColorValue clearColour;
	memset(&clearColour, 0, sizeof(VkClearColorValue));
	clearColour.float32[0] = 1.0f;
	clearColour.float32[1] = green;
	clearColour.float32[2] = 0.0f;
	clearColour.float32[3] = 0.0f;

	CommandBuffer * currentCommandBuffer = mDevice->GetCommandPool()->GetCurrentCommandBuffer();

	vkCmdClearColorImage(currentCommandBuffer->GetVkCommandBuffer(), GetVkImage(), VK_IMAGE_LAYOUT_GENERAL, &clearColour, 1, &subRange);
}





void VulkanImage::InsertImageMemoryBarrier(VkImageSubresourceRange& subRange, VkImageLayout newLayout)
{
	if (newLayout == GetLayout())
		return;

	CommandBuffer * currentCommandBuffer = mDevice->GetCommandPool()->GetCurrentCommandBuffer();
	currentCommandBuffer->BeginCommandBuffer();


	//Currently only 1 clear value.

	VkAccessFlags flags = {};
	if (newLayout == VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL)
		flags = VK_ACCESS_TRANSFER_WRITE_BIT;

	if (newLayout == VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL)
		flags = VK_ACCESS_TRANSFER_READ_BIT;



	VkImageMemoryBarrier memoryBarrier;
	memset(&memoryBarrier, 0, sizeof(VkImageMemoryBarrier));
	memoryBarrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
	memoryBarrier.pNext = nullptr;
	memoryBarrier.srcAccessMask = 0;
	memoryBarrier.dstAccessMask = flags;
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

	CommandBuffer * currentCommandBuffer = mDevice->GetCommandPool()->GetCurrentCommandBuffer();

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
	copyRegion.extent = { 400, 400, 1 };

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

	mStagingBuffer = new VulkanBuffer(mDevice);

	int x, y, n;
	unsigned char * data = stbi_load("Resources/jpeg_bad.jpg", &x, &y, &n, 4);

	unsigned char * pointer = (unsigned char*)malloc(x * y * 4);
	memset(pointer, 0, x * y * 4);
	
	for (int i = 0; i < x * y * 4; i = i + 4)
	{
		memset(pointer + i, 255, 1);
	}
	
	mStagingBuffer->LoadBufferData(data, x*y * 4);


	stbi_image_free(data);



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
	copyInfo.imageExtent = { 400, 400, 1};
	

	VkImageSubresourceRange barrierSubRange = {};

	barrierSubRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
	barrierSubRange.baseArrayLayer = 0;
	barrierSubRange.baseMipLevel = 0;
	barrierSubRange.layerCount = 1;
	barrierSubRange.levelCount = 1;


	CommandBuffer * currentCommandBuffer = mDevice->GetCommandPool()->GetCurrentCommandBuffer();
	currentCommandBuffer->BeginCommandBuffer();

	InsertImageMemoryBarrier(barrierSubRange, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL);

	vkCmdCopyBufferToImage(currentCommandBuffer->GetVkCommandBuffer(), mStagingBuffer->GetVkBuffer(), GetVkImage(), GetLayout(), 1, &copyInfo);
}





