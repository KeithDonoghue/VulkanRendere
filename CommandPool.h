#ifndef COMMAND_POOL_HDR
#define COMMAND_POOL_HDR 1

#include "ApiUsageHeader.h"

#include <deque>
#include <stack>

class VulkanDevice;
class CommandBuffer;



class CommandPool
{
public:
	CommandPool(VulkanDevice*);
	~CommandPool();
	void CreateCommandBuffer();
	VkCommandPool GetVkCommandPool() { return m_TheVulkanCommandPool; }
	VulkanDevice * GetVulkanDevice() { return mDevice; }
	CommandBuffer * GetCurrentCommandBuffer() { return mCurrentCommandBuffer; }
	CommandBuffer * NextCmdBuffer();

private:
	VkCommandPoolCreateInfo mCreateInfo;
	VulkanDevice * mDevice;

	VkCommandPool m_TheVulkanCommandPool;
	CommandBuffer * mCurrentCommandBuffer;
	std::deque<CommandBuffer*> mPendingList;
	std::stack<CommandBuffer*> mFreeStack;

};

#endif //  COMMAND_POOL_HDR
