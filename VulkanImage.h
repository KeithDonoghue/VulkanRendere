#ifndef VULKAN_IMAGE_HDR
#define VULKAN_IMAGE_HDR 1

#include "ApiUsageHeader.h"
#include "VulkanMemoryManager.h"

#include <vector>
#include <memory>

class VulkanDevice;
class VulkanBuffer;



typedef enum ImageType
{
	VULKAN_IMAGE_COLOR_RGBA8 = 0,
	VULKAN_IMAGE_DEPTH = 1,
	VULKAN_IMAGE_RANGE_SIZE = VULKAN_IMAGE_DEPTH - VULKAN_IMAGE_COLOR_RGBA8 + 1 
}ImageType;




class VulkanImage
{
public:
	VulkanImage(VulkanDevice*, VkImage, uint32_t, uint32_t, bool =  false);
	VulkanImage(VulkanDevice*, std::string);
	VulkanImage(VulkanDevice*, int width, int heigh, ImageType);
	void Init();
	~VulkanImage();
	VkImage GetVkImage() { return m_TheVulkanImage; }
	VkImage getVkImage() { return m_TheVulkanImage; }
	VkImageLayout GetLayout() { return mCurrentLayout; }
	VkFormat GetFormatForType(ImageType);
	VkFormat GetFormat();
	VkImageUsageFlags GetUsageForType(ImageType);
	VkAccessFlags	GetAccessFlags(VkImageLayout);
	void TransitionToClearable(VkImageSubresourceRange&);
	void TransToRecieveBlit();
	void BlittingFrom();


	void CopyImageData(VulkanImage&);
	void InsertImageMemoryBarrier(VkImageSubresourceRange&, VkImageLayout);
	void SetLayout(VkImageLayout layout) { mCurrentLayout = layout; }
	bool IsDirty(){ return mDirty; }

	void LoadDataToImage();
	void ClearImage(float);
	void ClearDepthImage(VkImageSubresourceRange&, float);
	void ClearColourImage(VkImageSubresourceRange&, float);
	void BlitFullImage(VulkanImage&);

	int getWidth(){ return mWidth; }
	int getHeight(){ return mHeight; }

private: // private methods.

	ImageType getType(){ return mType; }
	VkImageAspectFlags getAspectFlags();



private: // Private members.

	VkImage m_TheVulkanImage;
	VkImageCreateInfo mCreateInfo;
	VkImageLayout mCurrentLayout;
	VkExtent3D mExtent;

	ImageType mType;
	uint32_t mWidth, mHeight;

	VulkanDevice * mDevice;
	std::shared_ptr<VulkanBuffer> mStagingBuffer;
	bool mSystemManaged;
	allocation mAllocStruct;
	bool mDirty;
};

#endif // VULKAN_IMAGE_HDR