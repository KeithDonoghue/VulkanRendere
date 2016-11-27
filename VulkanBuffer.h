#include "vulkan/vulkan.h"

#include "VulkanMemoryManager.h"

class VulkanDevice;

class VulkanBuffer {
public:
	VulkanBuffer(VulkanDevice*);
	~VulkanBuffer();
	VkBuffer GetVkBuffer(){ return m_TheVulkanBuffer; }
	void BindMemory();
	void LoadBufferData(void *, uint32_t size);

private:
	bool TempMemoryBound;
	VkBuffer m_TheVulkanBuffer;
	VulkanDevice * mDevice;
	allocation mBlock;
};