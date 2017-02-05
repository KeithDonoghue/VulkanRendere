#ifndef COMMAND_BUFFER_HDR
#define COMMAND_BUFFER_HDR 1 

#include "vulkan/vulkan.h"
#include "RenderInstance.h"

#include <glm/mat4x4.hpp> // glm::mat4



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
	CommandBuffer(CommandPool&);
	void Init();
	void InitializeFence();
	~CommandBuffer();
	VkCommandBuffer GetVkCommandBuffer() { return m_TheVulkanCommandBuffer; }
	VkCommandBuffer * GetVkCommandBufferAddr() { return &m_TheVulkanCommandBuffer; }

	VkFence GetCompletionFence() { return mCompletionFence; }
	CommandPool& GetPool() { return mPool; }
	
	
	void GetImageReadyForPresenting(VulkanImage&);
	bool IsComplete();
	void BeginCommandBuffer();
	void EndCommandBuffer();

	void SetUpMVP(VulkanPipeline&, glm::mat4&);
	void StartDraw(RenderPass&, VulkanPipeline&, VulkanImage&, VkSampler, VkImageView);
	void Draw(VertexDraw);
	void Draw(IndexDraw);
	void EndDraw(RenderPass&, uint32_t);

private:
	VkCommandBufferAllocateInfo mAllocateInfo;
	VkCommandBuffer m_TheVulkanCommandBuffer;
	CommandBufferState mCommandBufferState;
	CommandPool& mPool;
	VkFence mCompletionFence;

	bool RenderPassCreated;
	VulkanBuffer * DrawBuffer;
};

#endif // COMMAND_BUFFER_HDR