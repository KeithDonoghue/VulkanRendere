#include "vulkan/vulkan.h"

#include "VulkanMemoryManager.h"

class VulkanDevice;

class VulkanBuffer {
public:
	VulkanBuffer(VulkanDevice&, VkBufferCreateInfo);
	~VulkanBuffer();
	VkBuffer GetVkBuffer(){ return m_TheVulkanBuffer; }
	VkBuffer * GetVkBufferAddr(){ return &m_TheVulkanBuffer; }
	void BindMemory();
	void LoadBufferData(const void *, uint32_t size);

	static VulkanBuffer * CreateStagingBuffer(VulkanDevice*, size_t);
	static VulkanBuffer * CreateVertexBuffer(VulkanDevice&, size_t);

private:
	bool TempMemoryBound;
	VkBuffer m_TheVulkanBuffer;
	VulkanDevice & mDevice;
	allocation mBlock;
};