#include "CommandBuffer.h"
#include "CommandPool.h"
#include "VulkanDevice.h"
#include "VulkanImage.h"
#include "VulkanBuffer.h"
#include "RenderPass.h"
#include "VulkanPipeline.h"

#include <cstring>

#include <glm/vec3.hpp> // glm::vec3
#include <glm/vec4.hpp> // glm::vec4
#include <glm/mat4x4.hpp> // glm::mat4
#include <glm/gtc/matrix_transform.hpp> // glm::translate, glm::rotate, glm::scale, glm::perspective



CommandBuffer::CommandBuffer(CommandPool& thePool) :
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
	mAllocateInfo.commandPool = mPool.GetVkCommandPool();
	mAllocateInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
	mAllocateInfo.commandBufferCount = 1;
	
	VkResult result = vkAllocateCommandBuffers(mPool.GetVulkanDevice()->getVkDevice(), &mAllocateInfo, &m_TheVulkanCommandBuffer);

	if (result != VK_SUCCESS)
	{
		EngineLog("Failed to allocate command buffer");
	}

	InitializeFence();
	InitializeSemaphore();
}





void CommandBuffer::InitializeFence()
{
	VkFenceCreateInfo createInfo = {};

	createInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
	createInfo.pNext = nullptr;
	createInfo.flags = 0; // VK_FENCE_CREATE_SIGNALED_BIT

	VkResult result = vkCreateFence(mPool.GetVulkanDevice()->getVkDevice(), &createInfo, nullptr, &mCompletionFence);

	if (result != VK_SUCCESS)
	{
		EngineLog("Failed to create Fence");
	}
}





void CommandBuffer::InitializeSemaphore()
{
	VkSemaphoreCreateInfo SemCreateInfo = {};
	SemCreateInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;

	vkCreateSemaphore(mPool.GetVulkanDevice()->getVkDevice(), &SemCreateInfo, nullptr, &mPendingSemaphore);
}






bool CommandBuffer::IsComplete()
{
	VkResult status  =  vkGetFenceStatus(mPool.GetVulkanDevice()->getVkDevice(), mCompletionFence);
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

	vkDestroyFence(mPool.GetVulkanDevice()->getVkDevice(), mCompletionFence, nullptr);
	vkDestroySemaphore(mPool.GetVulkanDevice()->getVkDevice(), mPendingSemaphore, nullptr);

	vkFreeCommandBuffers(mPool.GetVulkanDevice()->getVkDevice(), mPool.GetVkCommandPool(), 1, &m_TheVulkanCommandBuffer);
}





void CommandBuffer::BeginCommandBuffer()
{
	if (mCommandBufferState == CB_RECORDING_STATE)
	{
		return;
	}


	VkResult  result = vkResetFences(mPool.GetVulkanDevice()->getVkDevice(), 1, &mCompletionFence);

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





const VkSemaphore CommandBuffer::GetCompleteSignal() 
{ 
	mSignalSems.push_back(mPendingSemaphore);
	return mPendingSemaphore; 
}





void CommandBuffer::AddWaitSignal(VkSemaphore newSemWait)
{
	mWaitSems.push_back(newSemWait);
	mWaitStageFlags.push_back(VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT);
}





void CommandBuffer::AddWaitSignal(VkSemaphore newSemWait, VkPipelineStageFlagBits stageFlags)
{
	mWaitSems.push_back(newSemWait);
	mWaitStageFlags.push_back(stageFlags);
}





void CommandBuffer::AddFinishSignal(VkSemaphore newSemSignal)
{
	mSignalSems.push_back(newSemSignal);
}





void CommandBuffer::SubmitCommandBuffer()
{
	VkSubmitInfo theSubmitInfo = {};

	theSubmitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
	theSubmitInfo.pNext = nullptr;
	theSubmitInfo.waitSemaphoreCount = mWaitSems.size();
	theSubmitInfo.pWaitSemaphores = mWaitSems.data();
	theSubmitInfo.pWaitDstStageMask = mWaitStageFlags.data();
	theSubmitInfo.commandBufferCount = 1;
	theSubmitInfo.pCommandBuffers = GetVkCommandBufferAddr();
	theSubmitInfo.signalSemaphoreCount = mSignalSems.size();
	theSubmitInfo.pSignalSemaphores = mSignalSems.data();

	EndCommandBuffer();

	mPool.GetVulkanDevice()->LockQueue();
	VkResult result = vkQueueSubmit(mPool.GetVulkanDevice()->GetVkQueue(), 1, &theSubmitInfo, GetCompletionFence());
	mPool.GetVulkanDevice()->UnlockQueue();

	if (result != VK_SUCCESS)
	{
		EngineLog("Queue submission failed.");
	}

	mWaitSems.clear();
	mSignalSems.clear();
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





void CommandBuffer::SetDrawState(RenderPass&  theRenderPass,
	VulkanPipeline& thePipeline,
	VulkanImage& theImage,
	VkSampler theSampler,
	VkImageView theView)
{
	static bool dirty = true;

	vkCmdBindPipeline(GetVkCommandBuffer(), VK_PIPELINE_BIND_POINT_GRAPHICS, thePipeline.getVkPipeline());


	if (dirty)
	{
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
	}

	vkCmdBindDescriptorSets(GetVkCommandBuffer(),
		VK_PIPELINE_BIND_POINT_GRAPHICS,
		thePipeline.getLayout(),
		0,
		1,
		thePipeline.getDescSetAddr(),
		0,
		nullptr);

	VkViewport theViewport = theRenderPass.GetFrameBuffer().getFullViewPort();
	VkRect2D scissorRect = theRenderPass.GetFrameBuffer().getFBRect();

	vkCmdSetViewport(GetVkCommandBuffer(), 0, 1, &theViewport);
	vkCmdSetScissor(GetVkCommandBuffer(), 0, 1, &scissorRect);

}




void CommandBuffer::SetUpMVP(VulkanPipeline& thePipeline, glm::mat4 & MVP)
{

	float colour[] = { 1.0f, 0.0f, 0.0f, 1.0f };
	
	// glm perspecgive matrices will transform into NDC of z: -1(near) to +1(far), we need 0 to 1;
	//glm::mat4 correction = glm::scale(glm::mat4(), glm::vec3(1.0f, 1.0f, 0.5f));
	//correction = glm::translate(correction, glm::vec3(0.0f, 0.0f, 1.0f));
	//correction = glm::inverse(correction);

	vkCmdPushConstants(GetVkCommandBuffer(), thePipeline.getLayout(), VK_SHADER_STAGE_FRAGMENT_BIT , 0, sizeof(colour), colour);
	vkCmdPushConstants(GetVkCommandBuffer(), thePipeline.getLayout(), VK_SHADER_STAGE_VERTEX_BIT, 16, 64, &MVP[0]);
}






void CommandBuffer::SetInstanceData(VulkanPipeline& thePipeline, glm::mat4 * instanceData, uint32_t offset, uint32_t size)
{
	vkCmdPushConstants(GetVkCommandBuffer(), thePipeline.getLayout(), VK_SHADER_STAGE_VERTEX_BIT, offset, size, instanceData);
}





void CommandBuffer::Draw(VertexDraw theDraw)
{
	VkDeviceSize offset = 0;
	vkCmdBindVertexBuffers(GetVkCommandBuffer(), 0, 1, theDraw.mDrawBuffer->GetVkBufferAddr(), &offset);

	vkCmdDraw(GetVkCommandBuffer(), 
		theDraw.mVertexCount, 
		theDraw.mInstanceCount, 
		theDraw.mVertexOffset,
		theDraw.mInstanceOffset);
}





void CommandBuffer::Draw(IndexDraw theDraw)
{
	VkDeviceSize offset = 0;
	vkCmdBindVertexBuffers(GetVkCommandBuffer(), 0, 1, theDraw.mDrawBuffer->GetVkBufferAddr(), &offset);

	vkCmdBindIndexBuffer(GetVkCommandBuffer(), theDraw.mIndexBuffer->GetVkBuffer(), offset, VK_INDEX_TYPE_UINT32);

	vkCmdDrawIndexed(GetVkCommandBuffer(),
		theDraw.mIndexCount,
		theDraw.mInstanceCount,
		theDraw.mIndexOffset,
		theDraw.mVertexOffset,
		theDraw.mInstanceOffset);
}

