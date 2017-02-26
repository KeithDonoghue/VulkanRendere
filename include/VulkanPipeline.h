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
	VulkanPipeline(VulkanDevice&, VulkanPipeline &, VulkanRenderPass&, std::shared_ptr<ShaderModule>, std::shared_ptr<ShaderModule>);
	~VulkanPipeline();
	void Init();

	VkGraphicsPipelineCreateInfo & getCreateInfo(){ return mCreateInfo; }

	static VkGraphicsPipelineCreateInfo sCurrentCreateInfo;
	static VkGraphicsPipelineCreateInfo & getCurrentInfo(){ return sCurrentCreateInfo; }

	VkPipeline getVkPipeline(){ return m_TheVulkanPipeline; }
	VkPipelineLayout getLayout() { return mPipelineLayout; }
	VkDescriptorSet getDescSet() { return mDescSet; }
	VkDescriptorSet * getDescSetAddr() { return &mDescSet; }
	VkPipelineViewportStateCreateInfo * CreateViewportState(VkPipelineViewportStateCreateInfo*);
	VkPipelineDepthStencilStateCreateInfo * CreateDepthStencilState(VkPipelineDepthStencilStateCreateInfo*);
	VkPipelineRasterizationStateCreateInfo * CreateRasterisationState(VkPipelineRasterizationStateCreateInfo*);


	void CreatePipelineLayout(VulkanRenderPass&, ShaderModule&, ShaderModule&);
	std::vector<VkPipelineShaderStageCreateInfo> CreatePipelineShaderState(
		ShaderModule& vert,
		ShaderModule& frag);

private:
	VulkanDevice & mDevice;
	std::shared_ptr<ShaderModule> mVert, mFrag;
	VkGraphicsPipelineCreateInfo mCreateInfo;

	VkDescriptorSetLayout mSetLayout;
	VkDescriptorSet mDescSet;
	VkPipeline m_TheVulkanPipeline;
	VkPipelineLayout mPipelineLayout;



	//Initialisation stuff
	std::vector<VkPipelineShaderStageCreateInfo> stages;

	VkPipelineVertexInputStateCreateInfo VIScreateInfo = {};
	VkPipelineInputAssemblyStateCreateInfo IAcreateInfo = {};
	VkPipelineTessellationStateCreateInfo TScreateInfo = {};
	VkPipelineViewportStateCreateInfo VScreateInfo = {};
	VkPipelineRasterizationStateCreateInfo RScreateInfo = {};
	VkPipelineMultisampleStateCreateInfo MScreateInfo = {};
	VkPipelineDepthStencilStateCreateInfo DSScreateInfo = {};
	VkPipelineColorBlendStateCreateInfo CBScreateInfo = {};
	VkPipelineDynamicStateCreateInfo DScreateInfo = {};

	VkDynamicState dynamicStates[2];


	VkVertexInputBindingDescription VIBD = {};
	std::vector<VkVertexInputAttributeDescription> VIAD;

	VkPipelineColorBlendAttachmentState attachState = {};



};
#endif // VULKAN_PIPELINE_HPP 
