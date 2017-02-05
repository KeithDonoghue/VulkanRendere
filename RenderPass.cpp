#include "RenderPass.h"
#include "VulkanDevice.h"
#include "VulkanImage.h"

#define ENGINE_LOGGING_ENABLED 1
#include "EngineLogging.h"

RenderPass::RenderPass(VulkanDevice & theDevice, VulkanImage& depthImage, VulkanImage& colorImage):
mDevice(theDevice),
mDepthImage(depthImage),
mColorImage(colorImage),
mFramebuffer(*this, colorImage.getWidth(), colorImage.getHeight() , 1)
{

	VkAttachmentDescription attachments[2] = {};

	attachments[0].flags = 0; // VK_ATTACHMENT_DESCRIPTION_MAY_ALIAS_BIT 
	attachments[0].format = colorImage.GetFormat();
	attachments[0].samples = VK_SAMPLE_COUNT_1_BIT;
	attachments[0].loadOp = VK_ATTACHMENT_LOAD_OP_LOAD;
	attachments[0].storeOp = VK_ATTACHMENT_STORE_OP_STORE;
	attachments[0].stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
	attachments[0].stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
	attachments[0].initialLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
	attachments[0].finalLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
				
	attachments[1].flags = 0; // VK_ATTACHMENT_DESCRIPTION_MAY_ALIAS_BIT 
	attachments[1].format = depthImage.GetFormat();
	attachments[1].samples = VK_SAMPLE_COUNT_1_BIT;
	attachments[1].loadOp = VK_ATTACHMENT_LOAD_OP_LOAD;
	attachments[1].storeOp = VK_ATTACHMENT_STORE_OP_STORE;
	attachments[1].stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
	attachments[1].stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
	attachments[1].initialLayout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;
	attachments[1].finalLayout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;
	
			
	VkAttachmentReference color_reference = {};
	color_reference.attachment = 0;
	color_reference.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

	VkAttachmentReference depth_reference = {};
	
	depth_reference.attachment = 1;	
	depth_reference.layout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;
	
	VkSubpassDescription subpass = {};
	subpass.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
	subpass.flags = 0;
	subpass.inputAttachmentCount = 0;
	subpass.pInputAttachments = NULL;
	subpass.colorAttachmentCount = 1;
	subpass.pColorAttachments = &color_reference;
	subpass.pResolveAttachments = NULL;
	subpass.pDepthStencilAttachment = &depth_reference;
	subpass.preserveAttachmentCount = 0;
	subpass.pPreserveAttachments = NULL;


	VkRenderPassCreateInfo CreateInfo = {};
	CreateInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
	CreateInfo.pNext = nullptr;
	CreateInfo.flags = 0; //Reserved for future use.
	CreateInfo.attachmentCount = 2;
	CreateInfo.pAttachments = attachments;
	CreateInfo.subpassCount = 1;
	CreateInfo.pSubpasses = &subpass;
	CreateInfo.dependencyCount = 0;
	CreateInfo.pDependencies = nullptr;


	VkResult result = vkCreateRenderPass(GetVulkanDevice().GetVkDevice(), &CreateInfo, nullptr, &m_TheVulkanRenderPass);
	if (result != VK_SUCCESS)
	{
		EngineLog("failed to Create Render Pass.")
	}


	VkExtent2D RenderAreaGranularity;
	vkGetRenderAreaGranularity(mDevice.GetVkDevice(), GetVkRenderPass(), &RenderAreaGranularity);
	EngineLog("RenderAreaGranularity: \n   height: ", RenderAreaGranularity.height, "\n Width: ", RenderAreaGranularity.width);
	mFramebuffer.init(std::vector<VulkanImage*> {&colorImage, &depthImage});

}




RenderPass::~RenderPass()
{
	vkDestroyRenderPass(GetVulkanDevice().GetVkDevice(), m_TheVulkanRenderPass, nullptr);
}




VkRenderPassBeginInfo RenderPass::Begin()
{
	mDepthImage.ClearImage(1.0f);
	VkImageSubresourceRange subRange = {};

	subRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
	subRange.baseMipLevel = 0;
	subRange.levelCount = 1;
	subRange.baseArrayLayer = 0;
	subRange.layerCount = 1;

	mColorImage.InsertImageMemoryBarrier(subRange, VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL);

	subRange.aspectMask = VK_IMAGE_ASPECT_DEPTH_BIT;
	mDepthImage.InsertImageMemoryBarrier(subRange, VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL);

	subRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
	VkClearValue clearingValue = {};
	clearingValue.color.float32[0] = 1.0f;
	clearingValue.color.float32[1] = 1.0f;
	clearingValue.color.float32[2] = 1.0f;
	clearingValue.color.float32[3] = 1.0f;


	std::vector<VkClearValue> clearValues;
	clearValues.push_back(clearingValue);

	clearingValue = {};
	clearingValue.depthStencil = { 1.0f, 1.0f };
	clearValues.push_back(clearingValue);

	VkRect2D renderArea = {};
	renderArea.extent = { mFramebuffer.getWidth(), mFramebuffer.getHeight() };
	renderArea.offset = { 0, 0, };

	VkRenderPassBeginInfo BeginInfo = {};

	BeginInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
	BeginInfo.pNext = nullptr;
	BeginInfo.renderPass = GetVkRenderPass();
	BeginInfo.framebuffer = GetFrameBuffer().getVkFramebuffer();
	BeginInfo.renderArea = renderArea;
	BeginInfo.clearValueCount = clearValues.size();
	BeginInfo.pClearValues = clearValues.data();
	
	return BeginInfo;
}





void RenderPass::End()
{
	mDepthImage.ClearImage(0.5f);
}