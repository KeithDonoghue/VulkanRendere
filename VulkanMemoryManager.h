#ifndef VULKAN_MEM_MNGR_HDR
#define VULKAN_MEM_MNGR_HDR 1


#include "vulkan/vulkan.h"

#include "VulkanDevice.h"

#include <fstream>


typedef struct allocation {
	VkDeviceSize mSize;
	VkDeviceSize mOffset;
	VkDeviceMemory mPointer;
} allocation;
	


class VulkanDevice;

class VulkanMemMngr
{
public:
	VulkanMemMngr(VulkanDevice*);
	allocation GetAllocation(VkMemoryRequirements&, bool mappable = false);
	void FreeAllocation(allocation);
	uint32_t FindProperties(uint32_t, VkMemoryPropertyFlags);

private:
	std::ofstream mLog;
	VulkanDevice * mDevice;
	VkPhysicalDeviceMemoryProperties mMemProperties;
};

#endif // VULKAN_MEM_MNGR_HDR