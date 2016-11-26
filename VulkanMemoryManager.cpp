#include "VulkanMemoryManager.h"
#include "VulkanDevice.h"

#define ENGINE_LOGGING_ENABLE 1 
#include "EngineLogging.h"


VulkanMemMngr::VulkanMemMngr(VulkanDevice * theDevice):	
mDevice(theDevice),
mMemProperties{}
{

	mLog.open("MemoryLog.txt");

	vkGetPhysicalDeviceMemoryProperties(mDevice->GetVkPhysicalDevice(), &mMemProperties);

	mLog << "numHeaps: " << mMemProperties.memoryHeapCount << std::endl;
	mLog << "numTypes: " << mMemProperties.memoryTypeCount << std::endl;
	mLog << std::endl;
	mLog << "heaps: " << std::endl;

	for (int i = 0; i < mMemProperties.memoryHeapCount; i++)
	{
		mLog << "Heap " << i << ": " <<  std::endl;
		mLog << "	flags: " << mMemProperties.memoryHeaps[i].flags << std::endl;
		mLog << "	size: " << mMemProperties.memoryHeaps[i].size << std::endl;
	}

	for (int i = 0; i < mMemProperties.memoryTypeCount; i++)
	{
		mLog << "Heap " << i << ": " << std::endl;
		mLog << "	flags: " << mMemProperties.memoryTypes[i].propertyFlags << std::endl;
		mLog << "	index: " << mMemProperties.memoryTypes[i].heapIndex << std::endl;
	}
 
}




allocation VulkanMemMngr::GetAllocation(VkMemoryRequirements theRequirements, bool mappable)
{

	VkMemoryPropertyFlags required = 0;// VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT;

	allocation returnValue = {};
	
	VkMemoryAllocateInfo mAllocateInfo = {};
	EngineLog(theRequirements.memoryTypeBits);

	uint32_t memType = FindProperties( theRequirements.memoryTypeBits, required);
	
	EngineLog(memType);

	mAllocateInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
	mAllocateInfo.pNext = nullptr;
	mAllocateInfo.memoryTypeIndex = memType;
	mAllocateInfo.allocationSize = theRequirements.size;

	VkDeviceMemory deviceMem = {};
	VkResult result = vkAllocateMemory(mDevice->GetVkDevice(), &mAllocateInfo, nullptr, &deviceMem);

	if (result != VK_SUCCESS)
	{
		EngineLog("Failed to perform allocation.");
	}

	returnValue.mOffset = 0;
	returnValue.mPointer = deviceMem;
	returnValue.mSize = theRequirements.size;

	return returnValue;
}




void VulkanMemMngr::FreeAllocation(allocation theAllocation)
{
	vkFreeMemory(mDevice->GetVkDevice(), theAllocation.mPointer, nullptr);
}





// Find a memory type in "memoryTypeBits" that includes all of "properties" 
uint32_t VulkanMemMngr::FindProperties(uint32_t memoryTypeBits, VkMemoryPropertyFlags properties)
{
	for (uint32_t i = 0; i < mMemProperties.memoryTypeCount; ++i)
	{
		if ((memoryTypeBits & (1 << i)) && ((mMemProperties.memoryTypes[i].propertyFlags & properties) == properties))
			return i;
	}
	return -1;
}