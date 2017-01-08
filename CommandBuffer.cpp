#include "CommandBuffer.h"
#include "CommandPool.h"
#include "VulkanDevice.h"
#include "VulkanImage.h"
#include "VulkanBuffer.h"
#include "RenderPass.h"
#include "VulkanPipeline.h"

#define ENGINE_LOGGING_ENABLED 1
#include "EngineLogging.h"

#include <cstring>





CommandBuffer::CommandBuffer(CommandPool * thePool) :
mCommandBufferState(CB_INITIAL_STATE),
mPool(thePool),
DrawBuffer(nullptr)
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
	
	VkResult result = vkAllocateCommandBuffers(mPool->GetVulkanDevice()->getVkDevice(), &mAllocateInfo, &m_TheVulkanCommandBuffer);

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

	VkResult result = vkCreateFence(mPool->GetVulkanDevice()->getVkDevice(), &createInfo, nullptr, &mCompletionFence);

	if (result != VK_SUCCESS)
	{
		EngineLog("Failed to create Fence");
	}
}





bool CommandBuffer::IsComplete()
{
	VkResult status  =  vkGetFenceStatus(mPool->GetVulkanDevice()->getVkDevice(), mCompletionFence);
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

	delete DrawBuffer;

	vkDestroyFence(mPool->GetVulkanDevice()->getVkDevice(), mCompletionFence, nullptr);

	vkFreeCommandBuffers(mPool->GetVulkanDevice()->getVkDevice(), mPool->GetVkCommandPool(), 1, &m_TheVulkanCommandBuffer);
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


	VkResult  result = vkResetFences(mPool->GetVulkanDevice()->getVkDevice(), 1, &mCompletionFence);

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

	theImage.InsertImageMemoryBarrier(subRange, VK_IMAGE_LAYOUT_PRESENT_SRC_KHR);
}





void CommandBuffer::DoDraw(RenderPass&  theRenderPass, VulkanPipeline& thePipeline, VulkanImage& theImage, VkSampler theSampler, VkImageView theView)
{
	VkRenderPassBeginInfo beginInfo = theRenderPass.Begin();

	vkCmdBeginRenderPass(GetVkCommandBuffer(), &beginInfo, VK_SUBPASS_CONTENTS_INLINE);
	vkCmdBindPipeline(GetVkCommandBuffer(), VK_PIPELINE_BIND_POINT_GRAPHICS, thePipeline.getVkPipeline());

	if (DrawBuffer == nullptr)
	{
		const float vb[6][5] = {
			/*      position             texcoord */
			{ -0.5f, -1.0f, 0.25f, 1.0f, 0.0f },
			{ 0.5f, -1.0f, 0.25f, 1.0f, 1.0f },
			{ 0.5f, 1.0f, 1.0f, 0.5f, 1.0f },
			{ 0.5f, 1.0f, 1.0f, 0.5f, 1.0f },
			{ -0.5f, 1.0f, 1.0f, 0.5f, 1.0f },
			{ -0.5f, -1.0f, 0.25f, 0.0f, 0.0f },
		};

		DrawBuffer = VulkanBuffer::CreateVertexBuffer(theRenderPass.GetVulkanDevice(), sizeof(vb));
		DrawBuffer->LoadBufferData(vb, sizeof(vb));
	}

	VkWriteDescriptorSet writeDesc = {};
	VkDescriptorImageInfo imageInfo = {};

	imageInfo.imageLayout = theImage.GetLayout();
	imageInfo.imageView = theView;
	imageInfo.sampler = theSampler;


	writeDesc.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
	writeDesc.pNext = nullptr;
	writeDesc.dstSet = thePipeline.getDescSet();
	writeDesc.dstBinding = 0;
	writeDesc.dstArrayElement = 0;
	writeDesc.descriptorCount = 1;
	writeDesc.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
	writeDesc.pImageInfo = &imageInfo;

	vkUpdateDescriptorSets(theRenderPass.GetVulkanDevice().getVkDevice(), 1, &writeDesc, 0, nullptr);

	vkCmdBindDescriptorSets(GetVkCommandBuffer(), 
		VK_PIPELINE_BIND_POINT_GRAPHICS, 
		thePipeline.getLayout(), 
		0, 
		1, 
		thePipeline.getDescSetAddr(), 
		0, 
		nullptr);
	//vkCmdSetViewVport()
	//vkCmdSetScissor();

	VkDeviceSize offset = 0;
	vkCmdBindVertexBuffers(GetVkCommandBuffer(), 0, 1, DrawBuffer->GetVkBufferAddr(), &offset);
	/*
	VkViewport viewport;
	memset(&viewport, 0, sizeof(viewport));
	viewport.height = (float)400;
	viewport.width = (float)400;
	viewport.minDepth = (float)0.0f;
	viewport.maxDepth = (float)1.0f;
	vkCmdSetViewport(GetVkCommandBuffer(), 0, 1, &viewport);

	VkRect2D scissor;
	memset(&scissor, 0, sizeof(scissor));
	scissor.extent.width = 400;
	scissor.extent.height = 400;
	scissor.offset.x = 0;
	scissor.offset.y = 0;
	vkCmdSetScissor(GetVkCommandBuffer(), 0, 1, &scissor);
	*/
	vkCmdDraw(GetVkCommandBuffer(), 6, 1, 0, 0);	
	vkCmdEndRenderPass(GetVkCommandBuffer());

	theRenderPass.End();
}