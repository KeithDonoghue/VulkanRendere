#include "CommandBuffer.h"
#include "CommandPool.h"
#include "VulkanDevice.h"
#include "VulkanImage.h"

#define ENGINE_LOGGING_ENABLED 1
#include "EngineLogging.h"

#include <cstring>





CommandBuffer::CommandBuffer(CommandPool * thePool) :
mCommandBufferState(CB_INITIAL_STATE),
mPool(thePool)
{
	Init();
}




void CommandBuffer::Init()
{
	memset(&mAllocateInfo, 0, sizeof(VkCommandBufferAllocateInfo));

	mAllocateInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
	mAllocateInfo.pNext = nullptr;
	mAllocateInfo.commandPool = mPool->GetVkCommandPool();
	mAllocateInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
	mAllocateInfo.commandBufferCount = 1;
	
	VkResult result = vkAllocateCommandBuffers(mPool->GetVulkanDevice()->GetVkDevice(), &mAllocateInfo, &m_TheVulkanCommandBuffer);

	if (result != VK_SUCCESS)
	{
		EngineLog("Failed to allocate command buffer");
	}

	InitializeFence();
}





void CommandBuffer::InitializeFence()
{
	VkFenceCreateInfo createInfo = {};

	createInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
	createInfo.pNext = nullptr;
	createInfo.flags = 0; // VK_FENCE_CREATE_SIGNALED_BIT

	VkResult result = vkCreateFence(mPool->GetVulkanDevice()->GetVkDevice(), &createInfo, nullptr, &mCompletionFence);

	if (result != VK_SUCCESS)
	{
		EngineLog("Failed to create Fence");
	}
}





bool CommandBuffer::IsComplete()
{
	VkResult status  =  vkGetFenceStatus(mPool->GetVulkanDevice()->GetVkDevice(), mCompletionFence);
	if (status == VK_SUCCESS)
	{
		return true;
	}
	else if (status = VK_NOT_READY)
	{
		return false;
	}
	else
	{
		EngineLog("Error querying fence.");
		return false;
	}
}




CommandBuffer::~CommandBuffer()
{
	
	//Shouldn't be calling destructor without checking complete, but here for debugging.

	//vkWaitForFences(mPool->GetVulkanDevice()->GetVkDevice(), 1, &mCompletionFence, VK_TRUE, UINT64_MAX);

	vkDestroyFence(mPool->GetVulkanDevice()->GetVkDevice(), mCompletionFence, nullptr);

	vkFreeCommandBuffers(mPool->GetVulkanDevice()->GetVkDevice(), mPool->GetVkCommandPool(), 1, &m_TheVulkanCommandBuffer);
}





void CommandBuffer::CopyImage(VulkanImage& src, VulkanImage& dst)
{

	BeginCommandBuffer();


	VkImageCopy copyRegion = {};
		
	VkImageSubresourceLayers srcSubresource = {};
	VkImageSubresourceLayers dstSubresource = {};


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

	vkCmdCopyImage(GetVkCommandBuffer(),
		src.GetVkImage(), 
		VK_IMAGE_LAYOUT_GENERAL, 
		dst.GetVkImage(), 
		VK_IMAGE_LAYOUT_GENERAL, 
		1, 
		&copyRegion);

}





void CommandBuffer::BeginCommandBuffer()
{
	if (mCommandBufferState == CB_RECORDING_STATE)
	{
		return;
	}


	VkResult  result = vkResetFences(mPool->GetVulkanDevice()->GetVkDevice(), 1, &mCompletionFence);

	if (result != VK_SUCCESS)
	{
		EngineLog("Failed to reset fence");
	}



	VkCommandBufferBeginInfo BeginInfo;
	memset(&BeginInfo, 0, sizeof(VkCommandBufferBeginInfo));
	BeginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
	BeginInfo.pNext = nullptr;
	BeginInfo.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;
	BeginInfo.pInheritanceInfo = nullptr;

	result = vkBeginCommandBuffer(m_TheVulkanCommandBuffer, &BeginInfo);

	if (result != VK_SUCCESS)
	{
		EngineLog("failed to begin command buffer.")
	}
	else
	{
		mCommandBufferState = CB_RECORDING_STATE;
	}
}





void CommandBuffer::EndCommandBuffer()
{
	if (mCommandBufferState != CB_RECORDING_STATE)
	{
		EngineLog("Attempting to end command buffer not in recording state. Error.")
	}

	VkResult result = vkEndCommandBuffer(m_TheVulkanCommandBuffer);

	if (result != VK_SUCCESS)
	{
		EngineLog("failed to end command buffer.")
	}
	else
	{
		mCommandBufferState = CB_EXECUTABLE_STATE;
	}
}






void CommandBuffer::GetImageReadyForPresenting(VulkanImage& theImage)
{

	VkImageSubresourceRange subRange = {};

	subRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
	subRange.baseMipLevel = 0;
	subRange.levelCount = 1;
	subRange.baseArrayLayer = 0;
	subRange.layerCount = 1;
	VkImageMemoryBarrier PresentationMemoryBarrier = {};

	PresentationMemoryBarrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
	PresentationMemoryBarrier.pNext = nullptr;
	PresentationMemoryBarrier.srcAccessMask = VK_PIPELINE_STAGE_TRANSFER_BIT;
	PresentationMemoryBarrier.dstAccessMask = VK_ACCESS_MEMORY_READ_BIT;
	PresentationMemoryBarrier.oldLayout = theImage.GetLayout();
	PresentationMemoryBarrier.newLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;
	PresentationMemoryBarrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
	PresentationMemoryBarrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
	PresentationMemoryBarrier.image = theImage.GetVkImage();
	PresentationMemoryBarrier.subresourceRange = subRange;

	vkCmdPipelineBarrier(m_TheVulkanCommandBuffer, VK_PIPELINE_STAGE_ALL_COMMANDS_BIT, VK_PIPELINE_STAGE_BOTTOM_OF_PIPE_BIT,
		0, 0, nullptr, 0, nullptr, 1, &PresentationMemoryBarrier);

	theImage.SetLayout(VK_IMAGE_LAYOUT_PRESENT_SRC_KHR);

}