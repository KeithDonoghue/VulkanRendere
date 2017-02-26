#ifndef COMMAND_BUFFER_HDR
#define COMMAND_BUFFER_HDR 1 

#include "ApiUsageHeader.h"

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
	void InitializeSemaphore();
	~CommandBuffer();
	VkCommandBuffer GetVkCommandBuffer() { return m_TheVulkanCommandBuffer; }
	VkCommandBuffer * GetVkCommandBufferAddr() { return &m_TheVulkanCommandBuffer; }

	const VkFence GetCompletionFence() const { return mCompletionFence; }
	const VkSemaphore GetCompleteSignal(); 
	CommandPool& GetPool() { return mPool; }
	
	
	void AddWaitSignal(VkSemaphore);
	void AddWaitSignal(VkSemaphore, VkPipelineStageFlagBits);
	void AddFinishSignal(VkSemaphore);

	void GetImageReadyForPresenting(VulkanImage&);
	bool IsComplete();
	void BeginCommandBuffer();
	void EndCommandBuffer();
	void SubmitCommandBuffer();

	void SetUpMVP(VulkanPipeline&, glm::mat4&);
	void SetInstanceData(VulkanPipeline&, glm::mat4 *, uint32_t, uint32_t);
	void SetDrawState(VulkanRenderPass&, VulkanPipeline&, VulkanImage&, VkSampler);
	void Draw(VertexDraw);
	void Draw(IndexDraw);

private:
	VkCommandBufferAllocateInfo mAllocateInfo;
	VkCommandBuffer m_TheVulkanCommandBuffer;
	CommandBufferState mCommandBufferState;
	CommandPool& mPool;

	std::vector<VkSemaphore> mWaitSems;
	std::vector<VkPipelineStageFlags> mWaitStageFlags;
	std::vector<VkSemaphore> mSignalSems;
	VkFence mCompletionFence;
	VkSemaphore mPendingSemaphore;


	bool RenderPassCreated;
	VulkanBuffer * DrawBuffer;
};

#endif // COMMAND_BUFFER_HDR