#include "VulkanPipeline.h"
#include "VulkanDevice.h"
#include "RenderPass.h"
#include "ShaderModule.h"
#include "DescriptorPool.h"

#include "EngineLogging.h"

VulkanPipeline::VulkanPipeline(VulkanDevice& theDevice, RenderPass& theRenderPass, ShaderModule& vert, ShaderModule& frag) :
mDevice(theDevice)
{
	CreatePipelineLayout(theRenderPass, vert, frag);


	
	VkPipelineShaderStageCreateInfo stageCreateInfo = {};
	stageCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
	stageCreateInfo.pNext = nullptr;
	stageCreateInfo.flags = 0; // reserved for future use.
	stageCreateInfo.stage = VK_SHADER_STAGE_VERTEX_BIT;
	stageCreateInfo.module = vert.getVkShaderModule() ;
	stageCreateInfo.pName = "main";
	stageCreateInfo.pSpecializationInfo = nullptr;

	std::vector<VkPipelineShaderStageCreateInfo> stages;
	stages.push_back(stageCreateInfo);


	stageCreateInfo.stage = VK_SHADER_STAGE_FRAGMENT_BIT;
	stageCreateInfo.module = frag.getVkShaderModule();

	stages.push_back(stageCreateInfo);

	

	// Two stages: vs and fs
	VkPipelineShaderStageCreateInfo shaderStages[2];
	memset(&shaderStages, 0, 2 * sizeof(VkPipelineShaderStageCreateInfo));

	shaderStages[0].sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
	shaderStages[0].stage = VK_SHADER_STAGE_VERTEX_BIT;
	shaderStages[0].module = vert.getVkShaderModule();
	shaderStages[0].pName = "main";

	shaderStages[1].sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
	shaderStages[1].stage = VK_SHADER_STAGE_FRAGMENT_BIT;
	shaderStages[1].module = frag.getVkShaderModule();
	shaderStages[1].pName = "main";

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
	memset(dynamicStates, 0, sizeof(dynamicStates));
	dynamicStates[0] = VK_DYNAMIC_STATE_VIEWPORT;
	dynamicStates[1] = VK_DYNAMIC_STATE_SCISSOR;

	DScreateInfo.dynamicStateCount = 2;
	DScreateInfo.pDynamicStates = dynamicStates;


	VkGraphicsPipelineCreateInfo createInfo = {};
	createInfo.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
	createInfo.pNext = nullptr;
	createInfo.flags = VK_PIPELINE_CREATE_DISABLE_OPTIMIZATION_BIT;
	createInfo.stageCount = 2;
	createInfo.pStages = shaderStages;
	createInfo.pVertexInputState = &VIScreateInfo;
	createInfo.pInputAssemblyState = &IAcreateInfo;
	createInfo.pTessellationState = nullptr; // &TScreateInfo;
	createInfo.pViewportState = &VScreateInfo;
	createInfo.pRasterizationState = &RScreateInfo;
	createInfo.pMultisampleState = &MScreateInfo;
	createInfo.pDepthStencilState = &DSScreateInfo;
	createInfo.pColorBlendState = &CBScreateInfo;
	createInfo.pDynamicState = nullptr; &DScreateInfo; // &DScreateInfo;




	createInfo.layout = mPipelineLayout;
	createInfo.renderPass = theRenderPass.GetVkRenderPass();
	createInfo.subpass = 0;
	
	// Vertex Input state
	VkVertexInputBindingDescription VIBD = {};
	VIBD.binding = 0;
	VIBD.stride = 5 * sizeof(float);
	VIBD.inputRate = VK_VERTEX_INPUT_RATE_VERTEX;

	std::vector<VkVertexInputAttributeDescription> VIAD(2);

	VIAD[0].location = 0;
	VIAD[0].binding = 0;
	VIAD[0].format = VK_FORMAT_R32G32B32_SFLOAT;
	VIAD[0].offset = 0;

	VIAD[1].location = 1;
	VIAD[1].binding = 0;
	VIAD[1].format = VK_FORMAT_R32G32_SFLOAT;
	VIAD[1].offset = 3 * sizeof(float);


	VIScreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
	VIScreateInfo.vertexBindingDescriptionCount = 1;
	VIScreateInfo.pVertexBindingDescriptions = &VIBD;
	VIScreateInfo.vertexAttributeDescriptionCount = VIAD.size();
	VIScreateInfo.pVertexAttributeDescriptions = VIAD.data();



	// Input Assembly state
	IAcreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
	IAcreateInfo.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;



	// Viewport state
	VkViewport theViewport = {};
	theViewport.width = 400;
	theViewport.height = 400;
	theViewport.minDepth = 0.0f;
	theViewport.maxDepth = 1.0f;

	VkRect2D ScissorRect = {};
	ScissorRect.extent.width = 400;
	ScissorRect.extent.height = 400;
	ScissorRect.offset = { 0, 0 };

	VScreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
	VScreateInfo.viewportCount = 1;
	VScreateInfo.pViewports = &theViewport;
	VScreateInfo.scissorCount = 1;
	VScreateInfo.pScissors = &ScissorRect;


	//Rasterizer State
	RScreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
	RScreateInfo.polygonMode = VK_POLYGON_MODE_FILL;
	RScreateInfo.cullMode = VK_CULL_MODE_BACK_BIT;
	RScreateInfo.frontFace = VK_FRONT_FACE_CLOCKWISE;
	RScreateInfo.depthClampEnable = VK_FALSE;
	RScreateInfo.rasterizerDiscardEnable = VK_FALSE;
	RScreateInfo.depthBiasEnable = VK_FALSE;
	RScreateInfo.lineWidth = 1.0f;

	
	
	//Multisample state
	MScreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
	MScreateInfo.rasterizationSamples = VK_SAMPLE_COUNT_1_BIT;


	
	// Depth Stencil state
	DSScreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO;
	DSScreateInfo.depthTestEnable = VK_TRUE;
	DSScreateInfo.depthWriteEnable = VK_TRUE;
	DSScreateInfo.depthCompareOp = VK_COMPARE_OP_ALWAYS;
	DSScreateInfo.depthBoundsTestEnable = VK_FALSE;
	DSScreateInfo.back.failOp = VK_STENCIL_OP_KEEP;
	DSScreateInfo.back.passOp = VK_STENCIL_OP_KEEP;
	DSScreateInfo.back.compareOp = VK_COMPARE_OP_ALWAYS;
	DSScreateInfo.stencilTestEnable = VK_FALSE;
	DSScreateInfo.front = DSScreateInfo.back;
	

	//ColourBlend state
	VkPipelineColorBlendAttachmentState attachState = {};
	attachState.colorWriteMask = 0xF;
	attachState.blendEnable = VK_FALSE;

	CBScreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
	CBScreateInfo.attachmentCount = 1;
	CBScreateInfo.pAttachments = &attachState;


	// Dynamic State
	DScreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO;



	VkResult result = vkCreateGraphicsPipelines(mDevice.getVkDevice(), VK_NULL_HANDLE, 1, &createInfo, nullptr, &m_TheVulkanPipeline);
	if (result != VK_SUCCESS)
	{
		EngineLog("Failed to create pipeline.");
	}	
}





void VulkanPipeline::CreatePipelineLayout(RenderPass& theRenderPass, ShaderModule& vert, ShaderModule& frag)
{
	
	VkDescriptorSetLayoutBinding bindings = {};
	bindings.binding = 0;
	bindings.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
	bindings.descriptorCount = 1;
	bindings.stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT;
	bindings.pImmutableSamplers = nullptr;

	VkDescriptorSetLayoutCreateInfo layoutCreateInfo = {};
	layoutCreateInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
	layoutCreateInfo.pNext = nullptr;
	layoutCreateInfo.flags = 0; // reserved for future sue.
	layoutCreateInfo.bindingCount = 1;
	layoutCreateInfo.pBindings = &bindings;


	VkResult result = vkCreateDescriptorSetLayout(mDevice.getVkDevice(), &layoutCreateInfo, nullptr, &mSetLayout);
	if (result != VK_SUCCESS)
	{
		EngineLog("Failed to create descriptor set layout.");
	}


	mDescSet = mDevice.GetDescriptorPool().AllocateDescriptorSet(mSetLayout);

	VkPipelineLayoutCreateInfo pipelineLayoutCreateInfo = {};
	pipelineLayoutCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
	pipelineLayoutCreateInfo.pNext = nullptr;
	pipelineLayoutCreateInfo.flags = 0; // reserved for fuiture use.
	pipelineLayoutCreateInfo.setLayoutCount = 1;
	pipelineLayoutCreateInfo.pSetLayouts = &mSetLayout;
	pipelineLayoutCreateInfo.pushConstantRangeCount = 0;
	pipelineLayoutCreateInfo.pPushConstantRanges = nullptr;


	result = vkCreatePipelineLayout(mDevice.getVkDevice(), &pipelineLayoutCreateInfo, nullptr, &mPipelineLayout);
	if (result != VK_SUCCESS)
	{
		EngineLog("Failed to create pipeline layout.");
	}

}





VulkanPipeline::~VulkanPipeline()
{
	vkDestroyDescriptorSetLayout(mDevice.getVkDevice(), mSetLayout, nullptr);
	vkDestroyPipelineLayout(mDevice.getVkDevice(), mPipelineLayout, nullptr);
	vkDestroyPipeline(mDevice.getVkDevice(), m_TheVulkanPipeline, nullptr);
}