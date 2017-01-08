#include "VulkanBuffer.h"

#include <vulkan/vulkan.h>
#include "VulkanDevice.h"
#include "VulkanMemoryManager.h"

#include "EngineLogging.h"

#include <algorithm>




VulkanBuffer::VulkanBuffer(VulkanDevice& theDevice, VkBufferCreateInfo theBufferCreateInfo):
mDevice(theDevice),
TempMemoryBound(false)
{
	VkResult result = vkCreateBuffer(mDevice.getVkDevice(), &theBufferCreateInfo, nullptr, &m_TheVulkanBuffer);

	if (result != VK_SUCCESS)
	{
		EngineLog("Failed to create buffer");
	}
}





VulkanBuffer::~VulkanBuffer()
{	
	mDevice.GetMemManager()->FreeAllocation(mBlock);

	vkDestroyBuffer(mDevice.getVkDevice(), m_TheVulkanBuffer, nullptr);
}





void VulkanBuffer::LoadBufferData(const void * data, uint32_t size)
{
	BindMemory();

	void * mappedPointer;
	VkResult result = vkMapMemory(mDevice.getVkDevice(), mBlock.mPointer, mBlock.mOffset, mBlock.mSize, 0, &mappedPointer);

	if (result != VK_SUCCESS)
	{
		EngineLog("Failed To Map memory");
	}


	memcpy(mappedPointer, data, std::min<int>(size, mBlock.mSize));

	vkUnmapMemory(mDevice.getVkDevice(), mBlock.mPointer);

}





void VulkanBuffer::BindMemory()
{
	if (TempMemoryBound)
		return;
	VkMemoryRequirements memRequirements;

	vkGetBufferMemoryRequirements(mDevice.getVkDevice(), m_TheVulkanBuffer, &memRequirements);

	mBlock = mDevice.GetMemManager()->GetAllocation(memRequirements, true);

	VkResult result = vkBindBufferMemory(mDevice.getVkDevice(), m_TheVulkanBuffer, mBlock.mPointer, mBlock.mOffset);

	if (result != VK_SUCCESS)
	{
		EngineLog("Failed To bind Buffer memory");
	}
	else
	{
		TempMemoryBound = true;
	}

}





VulkanBuffer * VulkanBuffer::CreateStagingBuffer(VulkanDevice* theDevice, size_t size)
{

	VkBufferCreateInfo theBufferCreateInfo = {};
	theBufferCreateInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
	theBufferCreateInfo.pNext = nullptr;
	theBufferCreateInfo.size = 2000000;
	theBufferCreateInfo.usage = VK_BUFFER_USAGE_TRANSFER_SRC_BIT;
	theBufferCreateInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
	theBufferCreateInfo.queueFamilyIndexCount = 0;
	theBufferCreateInfo.pQueueFamilyIndices = nullptr;
	
	return new VulkanBuffer(*theDevice, theBufferCreateInfo);
}





VulkanBuffer * VulkanBuffer::CreateVertexBuffer(VulkanDevice& theDevice, size_t size)
{

	VkBufferCreateInfo theBufferCreateInfo = {};
	theBufferCreateInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
	theBufferCreateInfo.pNext = nullptr;
	theBufferCreateInfo.size = size;
	theBufferCreateInfo.usage = VK_BUFFER_USAGE_VERTEX_BUFFER_BIT;
	theBufferCreateInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
	theBufferCreateInfo.queueFamilyIndexCount = 0;
	theBufferCreateInfo.pQueueFamilyIndices = nullptr;

	return new VulkanBuffer(theDevice, theBufferCreateInfo);
}

