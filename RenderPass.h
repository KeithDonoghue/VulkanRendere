#include "vulkan/vulkan.h"



class RenderPass{

public:

	RenderPass(VkDevice);

private:

	VkRenderPassCreateInfo CreateInfo;
	VkDevice mDevice;
	VkRenderPass  mVulkanRenderPass;
};