#ifndef FRAMEBUFFER_HDR
#define FRAMEBUFFER_HDR

#include "vulkan/vulkan.h"


#include <vector>

class VulkanImage;
class RenderPass;

class FrameBuffer{
public: 
	FrameBuffer(RenderPass&, uint32_t, uint32_t, uint32_t);
	~FrameBuffer();
	VkFramebuffer getVkFramebuffer() { return m_TheVulkanFramebuffer; }
	void init(std::vector<VulkanImage*>);

private :
	VkFramebufferCreateInfo CreateInfo;
	RenderPass&				mRenderPass;
	std::vector<VkImageView> mAttachmentViews;

	VkFramebuffer			m_TheVulkanFramebuffer;
	uint32_t				mWidth, mHeight, mLayers;
};

#endif // FRAMEBUFFER_HDR
