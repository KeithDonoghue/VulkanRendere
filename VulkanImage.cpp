#include "VulkanImage.h"
#include "VulkanDevice.h"

#include "CommandPool.h"
#include "CommandBuffer.h"

#include "vulkan/vulkan.h"

#define ENGINE_LOGGING_ENABLED 1 
#include "EngineLogging.h"

VulkanImage::VulkanImage(VulkanDevice * theDevice, VkImage theImage, bool systemManaged) :
mDevice(theDevice),
m_TheVulkanImage(theImage),
mSystemManaged(systemManaged)
{}





VulkanImage::VulkanImage(VulkanDevice * theDevice, int width, int height):
mCreateInfo{},
mDevice(theDevice)
{
	mCreateInfo.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
	mCreateInfo.pNext = nullptr;
	mCreateInfo.flags = 0;
	mCreateInfo.imageType = VK_IMAGE_TYPE_2D;
	mCreateInfo.format = VK_FORMAT_B8G8R8A8_UNORM;
	mCreateInfo.extent = { width, height, 1 };
	mCreateInfo.mipLevels = 1;
	mCreateInfo.arrayLayers = 1;
	mCreateInfo.samples = VK_SAMPLE_COUNT_1_BIT;
	mCreateInfo.tiling = VK_IMAGE_TILING_LINEAR;
	mCreateInfo.usage = VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_TRANSFER_SRC_BIT;
	mCreateInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
	mCreateInfo.queueFamilyIndexCount = 0;
	mCreateInfo.pQueueFamilyIndices = nullptr;
	mCreateInfo.initialLayout = VK_IMAGE_LAYOUT_PREINITIALIZED;

	VkResult result = vkCreateImage(mDevice->GetVkDevice(), &mCreateInfo, nullptr, &m_TheVulkanImage);

	if (result != VK_SUCCESS)
	{
		EngineLog("Couldn't create VulkanImage");
	}


	mCurrentLayout = mCreateInfo.initialLayout;

	VkMemoryRequirements memoryRequirements = {};
	vkGetImageMemoryRequirements(mDevice->GetVkDevice(), m_TheVulkanImage, &memoryRequirements);

	mAllocStruct = mDevice->GetMemManager()->GetAllocation(memoryRequirements, true);

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
		mDevice->GetMemManager()->FreeAllocation(mAllocStruct);
		vkDestroyImage(mDevice->GetVkDevice(), m_TheVulkanImage, nullptr);
	}
}





void VulkanImage::ClearImage()
{

	CommandBuffer * currentCommandBuffer = mDevice->GetCommandPool()->GetCurrentCommandBuffer();
	currentCommandBuffer->BeginCommandBuffer();


	//Currently only 1 clear value.

	VkImageSubresourceRange subRange;
	memset(&subRange, 0, sizeof(VkImageSubresourceRange));

	subRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
	subRange.baseMipLevel = 0;
	subRange.levelCount = 1;
	subRange.baseArrayLayer = 0;
	subRange.layerCount = 1;

	VkImageMemoryBarrier memoryBarrier;
	memset(&memoryBarrier, 0, sizeof(VkImageMemoryBarrier));
	memoryBarrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
	memoryBarrier.pNext = nullptr;
	memoryBarrier.srcAccessMask = 0;
	memoryBarrier.dstAccessMask = VK_ACCESS_MEMORY_READ_BIT;
	memoryBarrier.oldLayout = GetLayout();
	memoryBarrier.newLayout = VK_IMAGE_LAYOUT_GENERAL;
	memoryBarrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
	memoryBarrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
	memoryBarrier.image = GetVkImage();
	memoryBarrier.subresourceRange = subRange;

	SetLayout(VK_IMAGE_LAYOUT_GENERAL);

	vkCmdPipelineBarrier(currentCommandBuffer->GetVkCommandBuffer(), VK_PIPELINE_STAGE_ALL_COMMANDS_BIT, VK_PIPELINE_STAGE_BOTTOM_OF_PIPE_BIT,
		0, 0, nullptr, 0, nullptr, 1, &memoryBarrier);


	VkClearColorValue clearColour;
	memset(&clearColour, 0, sizeof(VkClearColorValue));
	clearColour.float32[0] = 1.0f;
	clearColour.float32[1] = 0.0f;
	clearColour.float32[2] = 0.0f;
	clearColour.float32[3] = 0.0f;

	vkCmdClearColorImage(currentCommandBuffer->GetVkCommandBuffer(), GetVkImage(), VK_IMAGE_LAYOUT_GENERAL, &clearColour, 1, &subRange);
}





void VulkanImage::LoadDataToImage()
{

}