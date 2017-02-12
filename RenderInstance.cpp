#include "RenderInstance.h"
#include "VulkanImage.h"
#include "RenderPass.h"
#include "CommandPool.h"
#include "CommandBuffer.h"
#include "VulkanBuffer.h"


#define GLM_DEPTH_ZERO_TO_ONE 1
#include <glm/vec3.hpp> // glm::vec3
#include <glm/vec4.hpp> // glm::vec4
#include <glm/mat4x4.hpp> // glm::vec4
#include <glm/gtc/matrix_transform.hpp> // glm::translate, glm::rotate, glm::scale, glm::perspective


RenderInstance::RenderInstance(VulkanDevice&  theDevice, std::shared_ptr<VulkanPipeline> thePipeline, VulkanImage& theImage):
mPipeline(thePipeline),
mImage(theImage),
mDevice(theDevice),
mZPosition(0.0f),
mXPosition(0.0f),
mRotation(0.0f),
posDirty(true)
{

	VkSamplerCreateInfo  samplerCreateInfo = {};

	samplerCreateInfo.sType = VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO;
	samplerCreateInfo.pNext = NULL;
	samplerCreateInfo.magFilter = VK_FILTER_NEAREST;
	samplerCreateInfo.minFilter = VK_FILTER_NEAREST;
	samplerCreateInfo.mipmapMode = VK_SAMPLER_MIPMAP_MODE_NEAREST;
	samplerCreateInfo.addressModeU = VK_SAMPLER_ADDRESS_MODE_REPEAT;
	samplerCreateInfo.addressModeV = VK_SAMPLER_ADDRESS_MODE_REPEAT;
	samplerCreateInfo.addressModeW = VK_SAMPLER_ADDRESS_MODE_REPEAT;
	samplerCreateInfo.mipLodBias = 0.0f;
	samplerCreateInfo.anisotropyEnable = VK_FALSE;
	samplerCreateInfo.maxAnisotropy = 1;
	samplerCreateInfo.compareOp = VK_COMPARE_OP_NEVER;
	samplerCreateInfo.minLod = 0.0f;
	samplerCreateInfo.maxLod = 0.0f;
	samplerCreateInfo.borderColor = VK_BORDER_COLOR_FLOAT_OPAQUE_WHITE;
	samplerCreateInfo.unnormalizedCoordinates = VK_FALSE;

	VkResult result = vkCreateSampler(mDevice.getVkDevice(), &samplerCreateInfo, nullptr, &mSampler);
	if (result != VK_SUCCESS)
	{
		EngineLog("Failed to create Sampler");
	}

	VkImageViewCreateInfo view = {};
	view.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
	view.pNext = NULL;
	view.image = mImage.getVkImage();
	view.viewType = VK_IMAGE_VIEW_TYPE_2D;
	view.format = mImage.GetFormat();
	view.components =
	{
		VK_COMPONENT_SWIZZLE_R, VK_COMPONENT_SWIZZLE_G,
		VK_COMPONENT_SWIZZLE_B, VK_COMPONENT_SWIZZLE_A,
	};
	view.subresourceRange = { VK_IMAGE_ASPECT_COLOR_BIT, 0, 1, 0, 1 };
	view.flags = 0;

	result = vkCreateImageView(mDevice.getVkDevice(), &view, nullptr, &mView);
	if (result != VK_SUCCESS)
	{
		EngineLog("Failed to create Sampler");
	}
}





RenderInstance::~RenderInstance()
{
	vkDestroySampler(mDevice.getVkDevice(), mSampler, nullptr);
	vkDestroyImageView(mDevice.getVkDevice(), mView, nullptr);
}





void RenderInstance::Draw(RenderPass& theRenderPass)
{

	theRenderPass.Begin();


	CommandBuffer * currentCommandBuffer = mDevice.GetCommandPool().GetCurrentCommandBuffer();
	currentCommandBuffer->SetDrawState(theRenderPass, *mPipeline, mImage, mSampler, mView);
	UpdateMVP();
	UpdateRelModelMat();
	currentCommandBuffer->Draw(mIndexDraw);

	theRenderPass.End();
}





void RenderInstance::SetDrawBuffer(std::shared_ptr<VulkanBuffer> drawBuffer, uint32_t numPrimitives)
{
	mNumPrimitivesToRender = numPrimitives;
	mDrawBuffer = drawBuffer;
}





void RenderInstance::SetDraw(VertexDraw drawInfo)
{
	mVertexDraw = drawInfo;
}





void RenderInstance::SetDraw(IndexDraw drawInfo)
{
	mIndexDraw = drawInfo;
}





void RenderInstance::UpdateMVP()
{

	if (posDirty || 1)
	{
		posDirty = false;
		glm::mat4 Projection = glm::perspective(glm::radians(8.0f), 4.0f / 4.0f, 0.1f, 105.f);
		glm::mat4 View = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 0.0f, 0.0f));
		//View = glm::rotate(View, glm::radians(15.0f), glm::vec3(0.0f, -1.0f, 0.0f));
		glm::mat4 Model = glm::scale(glm::mat4(1.0f), glm::vec3(1.0f));
		Model = glm::translate(Model, glm::vec3(0.0f, mXPosition, mZPosition));
		Model = glm::rotate(Model, glm::radians(mRotation), glm::vec3(0.0f, 1.0f, 0.0f));
		MVP = Projection * View * Model;

		CommandBuffer * currentCommandBuffer = mDevice.GetCommandPool().GetCurrentCommandBuffer();
		currentCommandBuffer->SetUpMVP(*mPipeline, MVP);

	}
}




// Try to do this in as data orieted a way as possible.
void RenderInstance::UpdateRelModelMat()
{
	std::vector<glm::mat4> InstanceData(2);
	InstanceData[0] = glm::mat4();
	InstanceData[1] = glm::translate(InstanceData[0], glm::vec3(1.0, 0.0f, 0.0f));

	CommandBuffer * currentCommandBuffer = mDevice.GetCommandPool().GetCurrentCommandBuffer();
	currentCommandBuffer->SetInstanceData(*mPipeline, InstanceData.data(), 80, 128);
}