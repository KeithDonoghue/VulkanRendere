#ifndef VULKAN_RENDER_PASS_HDR
#define VULKAN_RENDER_PASS_HDR 1

#include "ApiUsageHeader.h"
#include "Framebuffer.h"

class VulkanDevice;

class VulkanRenderPass{

	typedef enum RenderPasstate{
		RenderPassOpen = 0,
		RenderPassClosed = 1
	}RenderPassState;

public:
	VulkanRenderPass(VulkanDevice &, VulkanImage&, VulkanImage&);
	~VulkanRenderPass();
	VulkanDevice & GetVulkanDevice(){ return mDevice; }
	FrameBuffer & GetFrameBuffer() { return mFramebuffer; }
	VkRenderPass GetVkRenderPass(){ return m_TheVulkanRenderPass; }
	void Begin();
	VkRenderPassBeginInfo getBeginInfo();
	void End();

private:
	VkRenderPassCreateInfo CreateInfo;
	VulkanDevice & mDevice;
	FrameBuffer mFramebuffer;
	VkRenderPass  m_TheVulkanRenderPass;
	VkFence mReadyFence;
	RenderPassState mCurrentState;

	VulkanImage& mDepthImage;
	VulkanImage& mColorImage;
};

#endif VULKAN_RENDER_PASS_HDR

