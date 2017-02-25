#ifndef VULKAN_PIPELINE_HPP 
#define VULKAN_PIPELINE_HPP 1

#include "ApiUsageHeader.h"


#include <memory>

class VulkanDevice;
class ShaderModule;
class VulkanRenderPass;

class VulkanPipeline
{
public:
	VulkanPipeline(VulkanDevice&, VulkanRenderPass&, std::shared_ptr<ShaderModule>, std::shared_ptr<ShaderModule>);
	~VulkanPipeline();

	VkPipeline getVkPipeline(){ return m_TheVulkanPipeline; }
	VkPipelineLayout getLayout() { return mPipelineLayout; }
	VkDescriptorSet getDescSet() { return mDescSet; }
	VkDescriptorSet * getDescSetAddr() { return &mDescSet; }
	VkPipelineViewportStateCreateInfo * CreateViewportState(VkPipelineViewportStateCreateInfo*);
	VkPipelineDepthStencilStateCreateInfo * CreateDepthStencilState(VkPipelineDepthStencilStateCreateInfo*);
	VkPipelineRasterizationStateCreateInfo * CreateRasterisationState(VkPipelineRasterizationStateCreateInfo*);

	void CreatePipelineLayout(VulkanRenderPass&, ShaderModule&, ShaderModule&);

private:
	std::shared_ptr<ShaderModule> mVert, mFrag;
	VkDescriptorSetLayout mSetLayout;
	VkDescriptorSet mDescSet;
	VkPipeline m_TheVulkanPipeline;
	VkPipelineLayout mPipelineLayout;
	VulkanDevice & mDevice;
};
#endif // VULKAN_PIPELINE_HPP 
