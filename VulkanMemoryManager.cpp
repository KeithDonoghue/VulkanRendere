#include "VulkanMemoryManager.h"
#include "VulkanDevice.h"

VulkanMemMngr::VulkanMemMngr(VkPhysicalDevice theDevice):	
mDevice(theDevice)
{

	mLog.open("MemoryLog.txt");

	VkPhysicalDeviceMemoryProperties theProperties = {};

	vkGetPhysicalDeviceMemoryProperties(mDevice, &theProperties);

	mLog << "numHeaps: " << theProperties.memoryHeapCount << std::endl;
	mLog << "numTypes: " << theProperties.memoryTypeCount << std::endl;
	mLog << std::endl;
	mLog << "heaps: " << std::endl;

	for (int i = 0; i < theProperties.memoryHeapCount; i++)
	{
		mLog << "Heap " << i << ": " <<  std::endl;
		mLog << "	flags: " << theProperties.memoryHeaps[i].flags << std::endl;
		mLog << "	size: " << theProperties.memoryHeaps[i].size << std::endl;
	}

	for (int i = 0; i < theProperties.memoryTypeCount; i++)
	{
		mLog << "Heap " << i << ": " << std::endl;
		mLog << "	flags: " << theProperties.memoryTypes[i].propertyFlags << std::endl;
		mLog << "	index: " << theProperties.memoryTypes[i].heapIndex << std::endl;
	}
 
}