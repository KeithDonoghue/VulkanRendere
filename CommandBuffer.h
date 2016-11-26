#include "vulkan/vulkan.h"

class CommandPool;
class VulkanImage;

typedef enum CommandBufferState{
	CB_INITIAL_STATE = 0,
	CB_RECORDING_STATE = 1,
	CB_EXECUTABLE_STATE = 2
} CommandBufferState;


class CommandBuffer
{
public:
	CommandBuffer(CommandPool *);
	VkCommandBuffer GetVkCommandBuffer() { return m_TheVulkanCommandBuffer; }
	VkCommandBuffer * GetVkCommandBufferAddr() { return &m_TheVulkanCommandBuffer; }
	void GetImageReadyForPresenting(VulkanImage&);
	void CopyImage(VulkanImage&, VulkanImage&);
	void BeginCommandBuffer();
	void EndCommandBuffer();

private:
	VkCommandBufferAllocateInfo mAllocateInfo;
	VkCommandBuffer m_TheVulkanCommandBuffer;
	CommandBufferState mCommandBufferState;
};