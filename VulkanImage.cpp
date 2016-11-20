#include "VulkanImage.h"
#include "VulkanDevice.h"

#include "vulkan/vulkan.h"

#define ENGINE_LOGGING_ENABLED 1 
#include "EngineLogging.h"

VulkanImage::VulkanImage(VkImage theImage, bool systemManaged) :
m_TheVulkanImage(theImage),
mSystemManaged(systemManaged)
{}




VulkanImage::VulkanImage(VulkanDevice * theDevice, int width, int height):
mCreateInfo{},
mDevice(theDevice)
{
	mCreateInfo.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
	mCreateInfo.pNext = nullptr;
	mCreateInfo.flags = 0;
	mCreateInfo.imageType = VK_IMAGE_TYPE_2D;
	mCreateInfo.format = VK_FORMAT_B8G8R8A8_UNORM;
	mCreateInfo.extent = { width, height, 1 };
	mCreateInfo.mipLevels = 1;
	mCreateInfo.arrayLayers = 1;
	mCreateInfo.samples = VK_SAMPLE_COUNT_1_BIT;
	mCreateInfo.tiling = VK_IMAGE_TILING_LINEAR;
	mCreateInfo.usage = VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_TRANSFER_SRC_BIT;
	mCreateInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
	mCreateInfo.queueFamilyIndexCount = 0;
	mCreateInfo.pQueueFamilyIndices = nullptr;
	mCreateInfo.initialLayout = VK_IMAGE_LAYOUT_PREINITIALIZED;

	VkResult result = vkCreateImage(mDevice->GetVkDevice(), &mCreateInfo, nullptr, &m_TheVulkanImage);

	if (result != VK_SUCCESS)
	{
		EngineLog("Couldn't create VulkanImage");
	}

	VkMemoryRequirements memoryRequirements = {};
	vkGetImageMemoryRequirements(mDevice->GetVkDevice(), m_TheVulkanImage, &memoryRequirements);

	mAllocStruct = mDevice->GetMemManager()->GetAllocation(memoryRequirements, true);

	result = vkBindImageMemory(mDevice->GetVkDevice(), m_TheVulkanImage, mAllocStruct.mPointer, mAllocStruct.mOffset);

	if (result != VK_SUCCESS)
	{
		EngineLog("Couldn't bind image memory.");
	}

}




VulkanImage::~VulkanImage()
{
	if (!mSystemManaged)
	{
		mDevice->GetMemManager()->FreeAllocation(mAllocStruct);
		vkDestroyImage(mDevice->GetVkDevice(), m_TheVulkanImage, nullptr);
	}
}