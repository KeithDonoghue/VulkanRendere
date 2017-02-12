#ifndef VULKAN_PIPELINE_HPP 
#define VULKAN_PIPELINE_HPP 1

#include "ApiUsageHeader.h"


class VulkanDevice;
class ShaderModule;
class RenderPass;

class VulkanPipeline
{
public:
	VulkanPipeline(VulkanDevice&, RenderPass&, ShaderModule&, ShaderModule&);
	~VulkanPipeline();

	VkPipeline getVkPipeline(){ return m_TheVulkanPipeline; }
	VkPipelineLayout getLayout() { return mPipelineLayout; }
	VkDescriptorSet getDescSet() { return mDescSet; }
	VkDescriptorSet * getDescSetAddr() { return &mDescSet; }
	VkPipelineViewportStateCreateInfo * CreateViewportState(VkPipelineViewportStateCreateInfo*);
	VkPipelineDepthStencilStateCreateInfo * CreateDepthStencilState(VkPipelineDepthStencilStateCreateInfo*);
	VkPipelineRasterizationStateCreateInfo * CreateRasterisationState(VkPipelineRasterizationStateCreateInfo*);

	void CreatePipelineLayout(RenderPass&, ShaderModule&, ShaderModule&);

private:
	VkDescriptorSetLayout mSetLayout;
	VkDescriptorSet mDescSet;
	VkPipeline m_TheVulkanPipeline;
	VkPipelineLayout mPipelineLayout;
	VulkanDevice & mDevice;
};
#endif // VULKAN_PIPELINE_HPP 
