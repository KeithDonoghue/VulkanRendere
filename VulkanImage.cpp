#include "VulkanImage.h"

VulkanImage::VulkanImage(VkImage theImage, bool systemManaged) :
m_TheVulkanImage(theImage),
mSystemManaged(systemManaged)
{}