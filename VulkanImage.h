#ifndef VULKAN_IMAGE_HDR
#define VULKAN_IMAGE_HDR 1

#include "vulkan/vulkan.h"

#include "VulkanMemoryManager.h"

#include <vector>

class VulkanDevice;

class VulkanImage
{
public:
	VulkanImage(VkImage, bool =  false);
	VulkanImage(VulkanDevice*, int width, int heigh);
	~VulkanImage();
	VkImage GetVkImage() { return m_TheVulkanImage; }

private:
	VkImage m_TheVulkanImage;
	VkImageCreateInfo mCreateInfo;

	VulkanDevice * mDevice;
	bool mSystemManaged;
	allocation mAllocStruct;
};

#endif // VULKAN_IMAGE_HDR