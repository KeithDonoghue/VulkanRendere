#include "CommandBuffer.h"
#include "CommandPool.h"
#include "VulkanDevice.h"
#include "VulkanImage.h"

#define ENGINE_LOGGING_ENABLED 1
#include "EngineLogging.h"

#include <cstring>

CommandBuffer::CommandBuffer(CommandPool * thePool)
{
	memset(&mAllocateInfo, 0, sizeof(VkCommandBufferAllocateInfo));

	mAllocateInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
	mAllocateInfo.pNext = nullptr;
	mAllocateInfo.commandPool = thePool->GetVkCommandPool();
	mAllocateInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
	mAllocateInfo.commandBufferCount = 1;
	
	VkResult result = vkAllocateCommandBuffers(thePool->GetVulkanDevice()->GetVkDevice(), &mAllocateInfo, &m_TheVulkanCommandBuffer);

	if (result != VK_SUCCESS)
	{
		EngineLog("Failed to allocate command buffer");
	}
}





void CommandBuffer::GetImageReadyForPresenting(VulkanImage theImage)
{
	VkCommandBufferBeginInfo BeginInfo;
	memset(&BeginInfo, 0, sizeof(VkCommandBufferBeginInfo));
	BeginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
	BeginInfo.pNext = nullptr;
	BeginInfo.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;
	BeginInfo.pInheritanceInfo = nullptr;

	VkResult result = vkBeginCommandBuffer(m_TheVulkanCommandBuffer, &BeginInfo);

	if (result != VK_SUCCESS)
	{
		EngineLog("failed to begin command buffer.")
	}

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
	memoryBarrier.oldLayout = VK_IMAGE_LAYOUT_UNDEFINED;
	memoryBarrier.newLayout = VK_IMAGE_LAYOUT_GENERAL;
	memoryBarrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
	memoryBarrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
	memoryBarrier.image = theImage.GetVkImage();
	memoryBarrier.subresourceRange = subRange;


	vkCmdPipelineBarrier(m_TheVulkanCommandBuffer, VK_PIPELINE_STAGE_ALL_COMMANDS_BIT, VK_PIPELINE_STAGE_BOTTOM_OF_PIPE_BIT,
		0, 0, nullptr, 0, nullptr, 1, &memoryBarrier);


	VkClearColorValue clearColour;
	memset(&clearColour, 0, sizeof(VkClearColorValue));
	clearColour.float32[0] = 1.0f;
	clearColour.float32[0] = 1.0f;
	clearColour.float32[0] = 0.0f;
	clearColour.float32[0] = 1.0f;

	vkCmdClearColorImage(m_TheVulkanCommandBuffer, theImage.GetVkImage(), VK_IMAGE_LAYOUT_GENERAL, &clearColour, 1, &subRange);


	VkImageMemoryBarrier PresentationMemoryBarrier;
	memset(&PresentationMemoryBarrier, 0, sizeof(VkImageMemoryBarrier));
	PresentationMemoryBarrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
	PresentationMemoryBarrier.pNext = nullptr;
	PresentationMemoryBarrier.srcAccessMask = VK_PIPELINE_STAGE_TRANSFER_BIT;
	PresentationMemoryBarrier.dstAccessMask = VK_ACCESS_MEMORY_READ_BIT;
	PresentationMemoryBarrier.oldLayout = VK_IMAGE_LAYOUT_GENERAL;
	PresentationMemoryBarrier.newLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;
	PresentationMemoryBarrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
	PresentationMemoryBarrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
	PresentationMemoryBarrier.image = theImage.GetVkImage();
	PresentationMemoryBarrier.subresourceRange = subRange;

	vkCmdPipelineBarrier(m_TheVulkanCommandBuffer, VK_PIPELINE_STAGE_ALL_COMMANDS_BIT, VK_PIPELINE_STAGE_BOTTOM_OF_PIPE_BIT,
		0, 0, nullptr, 0, nullptr, 1, &PresentationMemoryBarrier);

	result = vkEndCommandBuffer(m_TheVulkanCommandBuffer);

	if (result != VK_SUCCESS)
	{
		EngineLog("failed to end command buffer.")
	}
}