#ifndef DESCRIPTOR_POOL_HDR
#define DESCRIPTOR_POOL_HDR 1

#include "ApiUsageHeader.h"

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

#endif //  DESCRIPTOR_POOL_HDR
