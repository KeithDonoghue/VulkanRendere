#include "vulkan/vulkan.h"

#include <fstream>

class VulkanDevice;

class VulkanMemMngr
{
public:
	VulkanMemMngr(VkPhysicalDevice);

private:
	std::ofstream mLog;
	VkPhysicalDevice mDevice;
};