#include "CommandPool.h"
#include "CommandBuffer.h"
#include "VulkanDevice.h"


#include <cstring>

CommandPool::CommandPool(VulkanDevice* theVulkanDevice):
mDevice(theVulkanDevice)
{
	memset(&mCreateInfo, 0, sizeof(VkCommandPoolCreateInfo));
	mCreateInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
	mCreateInfo.pNext = nullptr;
	mCreateInfo.flags = VK_COMMAND_POOL_CREATE_TRANSIENT_BIT | VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;
	mCreateInfo.queueFamilyIndex = 0;

	VkResult result = vkCreateCommandPool(mDevice->getVkDevice(), &mCreateInfo, nullptr, &m_TheVulkanCommandPool);
	if (result != VK_SUCCESS)
	{
		EngineLog("Failed to create command pool")
	}

	CreateCommandBuffer();
}





CommandPool::~CommandPool()
{
	while (!mFreeStack.empty())
	{
		auto it = mFreeStack.top();
		delete it;
		mFreeStack.pop();
	}

	for (auto it : mPendingList)
	{
		delete it;
	}

	delete mCurrentCommandBuffer;

	vkDestroyCommandPool(mDevice->getVkDevice(), m_TheVulkanCommandPool, nullptr);
}





void CommandPool::CreateCommandBuffer()
{
	mCurrentCommandBuffer = new CommandBuffer(*this);
}




CommandBuffer * CommandPool::NextCmdBuffer()
{
	mCurrentCommandBuffer->SubmitCommandBuffer();

	mPendingList.push_back(mCurrentCommandBuffer);

	bool deleted = true;
	std::deque<CommandBuffer*>::iterator CBuffer = mPendingList.begin();

	while (CBuffer != mPendingList.end())
	{
		if ((*CBuffer)->IsComplete())
		{
			mFreeStack.push(*CBuffer);
			CBuffer = mPendingList.erase(CBuffer);
		}
		else
		{
			break;
		}
	}
	
	if (!mFreeStack.empty())
	{
		mCurrentCommandBuffer = mFreeStack.top();
		mFreeStack.pop();
	}
	else
	{
		mCurrentCommandBuffer = new CommandBuffer(mDevice->GetCommandPool());
	}

	return mCurrentCommandBuffer;
}