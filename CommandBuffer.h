#include "vulkan/vulkan.h"

class CommandPool;
class VulkanImage;

class CommandBuffer
{
public:
	CommandBuffer(CommandPool *);
	VkCommandBuffer GetVkCommandBuffer() { return m_TheVulkanCommandBuffer; }
	VkCommandBuffer * GetVkCommandBufferAddr() { return &m_TheVulkanCommandBuffer; }
	void GetImageReadyForPresenting(VulkanImage&);
	void RealGetImageReadyForPresenting(VulkanImage&);
	void CopyImage(VulkanImage&, VulkanImage&);
	void BeginCommandBuffer();
	void EndCommandBuffer();
	void ClearImage(VulkanImage&);

private:
	VkCommandBufferAllocateInfo mAllocateInfo;
	VkCommandBuffer m_TheVulkanCommandBuffer;
	bool mInRecordingState;
};