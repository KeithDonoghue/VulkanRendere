#include "vulkan/vulkan.h"

class VulkanDevice;

class DescriptorPool {
public:
	DescriptorPool(VulkanDevice&);
	~DescriptorPool();
	VkDescriptorSet AllocateDescriptorSet(VkDescriptorSetLayout&);
private:
	VkDescriptorPool	m_TheVulkanDescriptorPool;

	VulkanDevice& mDevice;
};