#ifndef RENDER_PASS_HDR
#define RENDER_PASS_HDR

#include "ApiUsageHeader.h"
#include "Framebuffer.h"

class VulkanDevice;

class RenderPass{

	typedef enum RenderPasstate{
		RenderPassOpen = 0,
		RenderPassClosed = 1
	}RenderPassState;

public:
	RenderPass(VulkanDevice &, VulkanImage&, VulkanImage&);
	~RenderPass();
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

#endif // RENDER_PASS_HDR
