#ifndef RENDER_PASS_HDR
#define RENDER_PASS_HDR

#include "vulkan/vulkan.h"
#include "Framebuffer.h"

class VulkanDevice;

class RenderPass{
public:
	RenderPass(VulkanDevice &, VulkanImage&, VulkanImage&);
	~RenderPass();
	VulkanDevice & GetVulkanDevice(){ return mDevice; }
	FrameBuffer & GetFrameBuffer() { return mFramebuffer; }
	VkRenderPass GetVkRenderPass(){ return m_TheVulkanRenderPass; }
	VkRenderPassBeginInfo Begin();
	void End();

private:
	VkRenderPassCreateInfo CreateInfo;
	VulkanDevice & mDevice;
	FrameBuffer mFramebuffer;
	VkRenderPass  m_TheVulkanRenderPass;

	VulkanImage& mDepthImage;
	VulkanImage& mColorImage;
};

#endif // RENDER_PASS_HDR
