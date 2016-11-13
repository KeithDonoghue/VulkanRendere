#include <Vulkan/vulkan.h>

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

private:
	VkCommandPoolCreateInfo mCreateInfo;
	VulkanDevice * mDevice;

	VkCommandPool m_TheVulkanCommandPool;
	CommandBuffer * mCurrentCommandBuffer;
};