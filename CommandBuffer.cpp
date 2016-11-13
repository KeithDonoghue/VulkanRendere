#include "CommandBuffer.h"
#include "CommandPool.h"
#include "VulkanDevice.h"

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