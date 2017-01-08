#include "FrameBuffer.h"
#include "RenderPass.h"
#include "VulkanImage.h"

#include "EngineLogging.h"


FrameBuffer::FrameBuffer(RenderPass & theRenderPass,
	uint32_t  width,
	uint32_t height,
	uint32_t layers):
mRenderPass(theRenderPass),
mWidth(width),
mHeight(height),
mLayers(layers)
{}





void FrameBuffer::init(std::vector<VulkanImage*> attachments)
{

	VkComponentMapping mapping = {};
	VkImageSubresourceRange subRange = {};

	subRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
	subRange.baseMipLevel = 0;
	subRange.levelCount = VK_REMAINING_MIP_LEVELS;
	subRange.baseArrayLayer = 0;
	subRange.layerCount = VK_REMAINING_ARRAY_LAYERS;


	VkImageViewCreateInfo ViewCreateInfo = {};
	ViewCreateInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
	ViewCreateInfo.pNext = nullptr;
	ViewCreateInfo.flags = 0; // reserved for future use.
	ViewCreateInfo.image = attachments[0]->GetVkImage();
	ViewCreateInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
	ViewCreateInfo.format = attachments[0]->GetFormat();
	ViewCreateInfo.components = mapping;
	ViewCreateInfo.subresourceRange = subRange;


	
	mAttachmentViews.resize(2);

	VkResult result = vkCreateImageView(mRenderPass.GetVulkanDevice().GetVkDevice(), &ViewCreateInfo, nullptr, &mAttachmentViews[0]);
	if (result != VK_SUCCESS)
	{
		EngineLog("Failed to create Image View")
	}

	subRange.aspectMask = VK_IMAGE_ASPECT_DEPTH_BIT;
	ViewCreateInfo.image = attachments[1]->GetVkImage();
	ViewCreateInfo.format = attachments[1]->GetFormat();
	ViewCreateInfo.subresourceRange = subRange;


	result = vkCreateImageView(mRenderPass.GetVulkanDevice().GetVkDevice(), &ViewCreateInfo, nullptr, &mAttachmentViews[1]);

	

	VkFramebufferCreateInfo createInfo = {};
	createInfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
	createInfo.pNext = nullptr;
	createInfo.flags = 0; // reserved for future use.
	createInfo.attachmentCount = 2;
	createInfo.pAttachments = mAttachmentViews.data();
	createInfo.renderPass = mRenderPass.GetVkRenderPass();
	createInfo.width = mWidth;
	createInfo.height = mHeight;
	createInfo.layers = mLayers;
	
	result = vkCreateFramebuffer(mRenderPass.GetVulkanDevice().GetVkDevice(), &createInfo, nullptr, &m_TheVulkanFramebuffer);
	if (result != VK_SUCCESS)
	{
		EngineLog("Failed to create framebuffer");
	}
}




FrameBuffer::~FrameBuffer()
{
	for (auto it : mAttachmentViews)
	{
		vkDestroyImageView(mRenderPass.GetVulkanDevice().getVkDevice(), it, nullptr);
	}

	vkDestroyFramebuffer(mRenderPass.GetVulkanDevice().getVkDevice(), m_TheVulkanFramebuffer, nullptr);

}