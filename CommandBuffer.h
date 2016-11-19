#include "vulkan/vulkan.h"

class CommandPool;
class VulkanImage;

class CommandBuffer
{
public:
	CommandBuffer(CommandPool *);
	VkCommandBuffer * GetVkCommandBuffer() { return &m_TheVulkanCommandBuffer; }
	void GetImageReadyForPresenting(VulkanImage);

private:
	VkCommandBufferAllocateInfo mAllocateInfo;
	VkCommandBuffer m_TheVulkanCommandBuffer;
};