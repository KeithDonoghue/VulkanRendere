#include "vulkan/vulkan.h"

#include "VulkanMemoryManager.h"

class VulkanDevice;

class VulkanBuffer {
public:
	VulkanBuffer(VulkanDevice*);
	~VulkanBuffer();
	void LoadBufferData(void *, uint32_t size);

private:
	VkBuffer m_TheVulkanBuffer;
	VulkanDevice * mDevice;
	allocation mBlock;
};