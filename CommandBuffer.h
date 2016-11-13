#include "vulkan/vulkan.h"

class CommandPool;

class CommandBuffer
{
public:
	CommandBuffer(CommandPool *);

private:
	VkCommandBufferAllocateInfo mAllocateInfo;
	VkCommandBuffer m_TheVulkanCommandBuffer;
};