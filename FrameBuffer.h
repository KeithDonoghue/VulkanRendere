#ifndef FRAMEBUFFER_HDR
#define FRAMEBUFFER_HDR

#include "ApiUsageHeader.h"

#include <vector>

class VulkanImage;
class RenderPass;

class FrameBuffer{
public: 
	FrameBuffer(RenderPass&, uint32_t, uint32_t, uint32_t);
	~FrameBuffer();
	VkFramebuffer getVkFramebuffer() { return m_TheVulkanFramebuffer; }
	void init(std::vector<VulkanImage*>);
	VkViewport	getFullViewPort();
	VkRect2D	getFBRect();

	uint32_t getWidth(){ return mWidth; }
	uint32_t getHeight(){ return mHeight; }

private: //members
	VkFramebufferCreateInfo CreateInfo;
	RenderPass&				mRenderPass;
	std::vector<VkImageView> mAttachmentViews;

	VkFramebuffer			m_TheVulkanFramebuffer;
	uint32_t				mWidth, mHeight, mLayers;
};

#endif // FRAMEBUFFER_HDR
