#ifndef VULKAN_IMAGE_HDR
#define VULKAN_IMAGE_HDR 1

#include "vulkan/vulkan.h"

#include <vector>


class VulkanImage
{
public:
	VulkanImage(VkImage, bool =  false);
	VkImage GetVkImage() { return m_TheVulkanImage; }

private:
	VkImage m_TheVulkanImage;

	bool mSystemManaged;
};

#endif // VULKAN_IMAGE_HDR