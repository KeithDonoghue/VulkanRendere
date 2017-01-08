#include "vulkan/vulkan.h"

class CommandPool;
class VulkanImage;
class VulkanDevice;
class RenderPass;
class VulkanPipeline;
class VulkanBuffer;

typedef enum CommandBufferState{
	CB_INITIAL_STATE = 0,
	CB_RECORDING_STATE = 1,
	CB_EXECUTABLE_STATE = 2
} CommandBufferState;


class CommandBuffer
{
public:
	CommandBuffer(CommandPool *);
	void Init();
	void InitializeFence();
	~CommandBuffer();
	VkCommandBuffer GetVkCommandBuffer() { return m_TheVulkanCommandBuffer; }
	VkCommandBuffer * GetVkCommandBufferAddr() { return &m_TheVulkanCommandBuffer; }

	VkFence GetCompletionFence() { return mCompletionFence; }
	CommandPool * GetPool() { return mPool; }
	
	
	void GetImageReadyForPresenting(VulkanImage&);
	void CopyImage(VulkanImage&, VulkanImage&);
	bool IsComplete();
	void BeginCommandBuffer();
	void EndCommandBuffer();

	void DoDraw(RenderPass&, VulkanPipeline&, VulkanImage&, VkSampler, VkImageView);


private:
	VkCommandBufferAllocateInfo mAllocateInfo;
	VkCommandBuffer m_TheVulkanCommandBuffer;
	CommandBufferState mCommandBufferState;
	CommandPool * mPool;
	VkFence mCompletionFence;

	bool RenderPassCreated;
	RenderPass * theRenderPass;

	VulkanBuffer * DrawBuffer;
};