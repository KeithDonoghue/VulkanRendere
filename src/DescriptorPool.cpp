#include "DescriptorPool.h"
#include "VulkanDevice.h"

DescriptorPool::DescriptorPool(VulkanDevice& theDevice):
mDevice(theDevice)
{
	VkDescriptorPoolCreateInfo poolCreateInfo = {};
	VkDescriptorPoolSize descType = {};
	descType.type = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
	descType.descriptorCount = 3;

	poolCreateInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
	poolCreateInfo.pNext = nullptr;
	poolCreateInfo.flags = VK_DESCRIPTOR_POOL_CREATE_FREE_DESCRIPTOR_SET_BIT;
	poolCreateInfo.maxSets = 3;
	poolCreateInfo.poolSizeCount = 1;
	poolCreateInfo.pPoolSizes = &descType;

	VkResult result = vkCreateDescriptorPool(mDevice.getVkDevice(), &poolCreateInfo, nullptr, &m_TheVulkanDescriptorPool);
}





DescriptorPool::~DescriptorPool()
{
	vkDestroyDescriptorPool(mDevice.getVkDevice(), m_TheVulkanDescriptorPool, nullptr);
}




VkDescriptorSet DescriptorPool::AllocateDescriptorSet(VkDescriptorSetLayout& theLayout)
{
	VkDescriptorSet returnSet;

	VkDescriptorSetAllocateInfo allocInfo = {};
	allocInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
	allocInfo.pNext = nullptr;
	allocInfo.descriptorPool = m_TheVulkanDescriptorPool;
	allocInfo.descriptorSetCount = 1;
	allocInfo.pSetLayouts = &theLayout;

	vkAllocateDescriptorSets(mDevice.getVkDevice(), &allocInfo, &returnSet);

	return returnSet;
}