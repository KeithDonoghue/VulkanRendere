#include "vulkan/vulkan.h"

class FrameBuffer{

public: 

	FrameBuffer(VkDevice);

private :
	VkFramebufferCreateInfo CreateInfo;
	VkDevice				mDevice;
	VkFramebuffer			mVulkanFramebuffer;
};